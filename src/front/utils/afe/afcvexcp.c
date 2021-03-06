/*
**	Copyright (c) 1989 Ingres Corporation
*/

# include	<compat.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include	<ex.h>
# include	<er.h>
# include	<adf.h>
# include	<afe.h>


/**
** Name:	afcvexcp.c	- Conversion with built-in handler.
**
** Description:
**	This file defines:
**
**	IIARcwhConvertWithHandler - Does a conversion handling exceptions.
**
**	The base routine that does conversion in AFE is afe_cvinto.  afe_cvinto
**	does not handle any exceptions that can occur during conversion.
**	Instead it depends on the caller catching the exceptions explicitly.
**	This is normally fine since the callers of afe_cvinto like the FRS
**	and the 4GL interpreter have a handler declared in these cases.
**	However, some callers would like the conversion routine to
**	return any problems as an error, and that is what this
**	routine is for.
**
** History:
**	29-nov-1989 (Joe)
**	    First Written
**	23-sep-91 (seg)
**	    clear up re-declaration of IIAFcehConversionError problem by
**	    redeclaring as static and providing forward declaration.  Also,
**	    The EX handler returns a STATUS, not an EX.
**	5/92 (Mike S) IIAFcehConversionError is called from outside
**		this module.  Redeclare it as external.
**	22-jul-97 (phach01)
**	    Handle decimal overflow. 
**/

/* # define's */
/* GLOBALDEF's */
/* extern's */
/* static's */
static ADF_CB	 *adfcb;	/* Shared by the handler/conversion routine */
static DB_STATUS db_status;

static STATUS cvthandler();
FUNC_EXTERN STATUS IIAFcehConversionErrorHandler();

/*{
** Name:	IIAFcwhConvertWithHandler	- convert.
**
**  Description:
**
**  Inputs:
**	acb			Pointer to the current ADF_CB control block.
**	idbv			Input DB_DATA_VALUE with source data:
**	   .db_datatype		Data type.
**	   .db_length		Data length.
**	   .db_prec		Precision (unused right now).
**	   .db_data		Input value.
**	odbv			Output DB_DATA_VALUE with description:
**	   .db_datatype		Data type.
**	   .db_length		Data length.
**	   .db_prec		Precision (unused right now).
**
**  Outputs:
**	acb.adf_errcb		ADF error data (if errroneous conversion):
**	   .ad_errcode		Error code if failure.  See numbers below.
**	   .ad_emsglen		If an error occured this is set to zero as
**				no message is looked up.
**	odbv			Output DB_DATA_VALUE with result buffer:
**	   .db_data		To be filled with converted/copied data.
**
** Returns:
**	{DB_STATUS}    	E_DB_OK		If succeeded.
**	    		E_DB_ERROR	If error occurred in the conversion.
**	   	 	E_DB_WARN	If warning occurred in the conversion.
** Errors:
**	ADF errors reported from afe_cvinto
**	Plus these errors:
**		Note that these have no error text associated with them.
**	
** 		E_AD1121_INTOVF_ERROR;
**		E_AD1123_FLTOVF_ERROR;
**		E_AD1124_FLTUND_ERROR;
**
**  History:
**	29-nov-1989 (Joe)
**	    First Written
*/
DB_STATUS
IIAFcwhConvertWithHandler(acb, idbv, odbv)
ADF_CB		*acb;
DB_DATA_VALUE	*idbv;
DB_DATA_VALUE	*odbv;
{
    DB_STATUS	rval;
    EX_CONTEXT	context;

    FUNC_EXTERN DB_STATUS	afe_cvinto();

    adfcb = acb;
    if (EXdeclare(cvthandler, &context) != OK)
    {
	/*
	** The handler will set the status in the adfcb given
	** by adfcb.
	*/
	EXdelete();
	return db_status;
    }
    rval = afe_cvinto(acb, idbv, odbv);
    EXdelete();
    return rval;
}

/*{
** Name:	cvthandler	- Handler for IIARcwhConvertWithHandler
**
** Description:
**	Use IIARcehConversionErrorHandler.
**
** Inputs:
**	ex		The exception argument structure.
**
** Outputs:
**	Returns:
**		An exception value of whether to resignal the exception
**		or not.
**
** Side Effects:
**	Passes the statics adfcb and db_status to IIARcehConversionErrorHandler.
**
** History:
**	29-nov-1989 (Joe)
**	    First Written
*/
static STATUS
cvthandler(ex)
EX_ARGS *ex;
{
    return IIAFcehConversionErrorHandler(ex, adfcb, &db_status);
}

/*{
** Name:	IIARcehConversionErrorHandler	- handle errors on convert.
**
** Description:
**	This is designed to be called from an EX exception handler that
**	has caught an event generated by an afe_cvinto.  The routine takes
**	the exception arguments and determines if the exception it
**	represents is an possible error during the conversion.  If it
**	is not, then the routine returns EXRESIGNAL.
**	If it is, then the routine converts the exception into an error
**	code that is put into the error structure within the ADF control
**	block.
**
**	The idea is that this routine can be used with an exception handler
**	being used to catch an exceptions generated by an afe_cvinto as in:
**
**	static ADF_CB	*shared_adf_cb;
**	static DB_STATUE shared_status;
**
**	caller_of_afecvinto()
**	{
**	   ...
**	    if (EXdeclare(myhandler, &my_context) != OK)
**	    {
**	    }
**	  ...
**	    afe_cvinto(...);
**	  ...
**	  EXdelete();
**	}
**
**	myhandler()
**	{
**	    return IIARcehConversionErrorHandler(ex, shared_adf_cb,
**						 &shared_status);
**	}
**
**	Of course there are other ways to use it...
**	
** Inputs:
**	ex		An EX_ARG.  It is examine to see if
**			it contains a possible exception from an
**			afe_cvinto call.  Currently this recognizes
**			integer overflow
**			floating overflow
**			floating underflow.
**
**	acb		An ADF control block.
**
**	ret_status	A pointer to a DB_STATUS.
**			
** Outputs:
**	acb
**	    .adf_errcb	If ex contains one of the recognized conversion
**			errors then this will be set to the error
**			that corresponds to the exception.
**	
**	ret_status	If ex contains one of the recognized conversion
**			exceptions then this will be set to the E_DB
**			error status for the corresponding error.
**			Otherwise E_DB_OK
**
**	Returns:
**		An EX exception code that can legally be returned
**		from an exception handler.  If ex contained a recognized
**		conversion exception then the return will be EXDECLARE
**		otherwise it will be EXRESIGNAL.
**
** History:
**	29-nov-1989 (Joe)
**	    First Written
**	23-sep-91 (seg)
**	    clear up declaration problem by redeclaring as static
*/
STATUS
IIAFcehConversionErrorHandler(ex, acb, ret_status)
EX_ARGS		*ex;
ADF_CB		*acb;
DB_STATUS	*ret_status;
{
    ER_MSGID	msg_id;

    *ret_status = E_DB_OK;

    switch (ex->exarg_num)
    {
      case EXINTOVF:
	msg_id = E_AD1121_INTOVF_ERROR;
	break;

      case EXFLTOVF:
	msg_id = E_AD1123_FLTOVF_ERROR;
	break;

      case EXFLTUND:
	msg_id = E_AD1124_FLTUND_ERROR;
	break;

      case EXDECOVF:
	msg_id = E_AD1127_DECOVF_ERROR;
	break;

      default:
	    return(EXRESIGNAL);
    }
    *ret_status = afe_error(acb, msg_id, 0);
    return(EXDECLARE);
}
