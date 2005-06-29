
/*******************************************************************************
 *
 * Module Name: nsaccess - Top-level functions for accessing ACPI namespace
 *              $Revision: 1.105 $
 *
 ******************************************************************************/

/******************************************************************************
 *
 * 1. Copyright Notice
 *
 * Some or all of this work - Copyright (c) 1999, Intel Corp.  All rights
 * reserved.
 *
 * 2. License
 *
 * 2.1. This is your license from Intel Corp. under its intellectual property
 * rights.  You may have additional license terms from the party that provided
 * you this software, covering your right to use that party's intellectual
 * property rights.
 *
 * 2.2. Intel grants, free of charge, to any person ("Licensee") obtaining a
 * copy of the source code appearing in this file ("Covered Code") an
 * irrevocable, perpetual, worldwide license under Intel's copyrights in the
 * base code distributed originally by Intel ("Original Intel Code") to copy,
 * make derivatives, distribute, use and display any portion of the Covered
 * Code in any form, with the right to sublicense such rights; and
 *
 * 2.3. Intel grants Licensee a non-exclusive and non-transferable patent
 * license (with the right to sublicense), under only those claims of Intel
 * patents that are infringed by the Original Intel Code, to make, use, sell,
 * offer to sell, and import the Covered Code and derivative works thereof
 * solely to the minimum extent necessary to exercise the above copyright
 * license, and in no event shall the patent license extend to any additions
 * to or modifications of the Original Intel Code.  No other license or right
 * is granted directly or by implication, estoppel or otherwise;
 *
 * The above copyright and patent license is granted only if the following
 * conditions are met:
 *
 * 3. Conditions
 *
 * 3.1. Redistribution of Source with Rights to Further Distribute Source.
 * Redistribution of source code of any substantial portion of the Covered
 * Code or modification with rights to further distribute source must include
 * the above Copyright Notice, the above License, this list of Conditions,
 * and the following Disclaimer and Export Compliance provision.  In addition,
 * Licensee must cause all Covered Code to which Licensee contributes to
 * contain a file documenting the changes Licensee made to create that Covered
 * Code and the date of any change.  Licensee must include in that file the
 * documentation of any changes made by any predecessor Licensee.  Licensee
 * must include a prominent statement that the modification is derived,
 * directly or indirectly, from Original Intel Code.
 *
 * 3.2. Redistribution of Source with no Rights to Further Distribute Source.
 * Redistribution of source code of any substantial portion of the Covered
 * Code or modification without rights to further distribute source must
 * include the following Disclaimer and Export Compliance provision in the
 * documentation and/or other materials provided with distribution.  In
 * addition, Licensee may not authorize further sublicense of source of any
 * portion of the Covered Code, and must include terms to the effect that the
 * license from Licensee to its licensee is limited to the intellectual
 * property embodied in the software Licensee provides to its licensee, and
 * not to intellectual property embodied in modifications its licensee may
 * make.
 *
 * 3.3. Redistribution of Executable. Redistribution in executable form of any
 * substantial portion of the Covered Code or modification must reproduce the
 * above Copyright Notice, and the following Disclaimer and Export Compliance
 * provision in the documentation and/or other materials provided with the
 * distribution.
 *
 * 3.4. Intel retains all right, title, and interest in and to the Original
 * Intel Code.
 *
 * 3.5. Neither the name Intel nor any other trademark owned or controlled by
 * Intel shall be used in advertising or otherwise to promote the sale, use or
 * other dealings in products derived from or relating to the Covered Code
 * without prior written authorization from Intel.
 *
 * 4. Disclaimer and Export Compliance
 *
 * 4.1. INTEL MAKES NO WARRANTY OF ANY KIND REGARDING ANY SOFTWARE PROVIDED
 * HERE.  ANY SOFTWARE ORIGINATING FROM INTEL OR DERIVED FROM INTEL SOFTWARE
 * IS PROVIDED "AS IS," AND INTEL WILL NOT PROVIDE ANY SUPPORT,  ASSISTANCE,
 * INSTALLATION, TRAINING OR OTHER SERVICES.  INTEL WILL NOT PROVIDE ANY
 * UPDATES, ENHANCEMENTS OR EXTENSIONS.  INTEL SPECIFICALLY DISCLAIMS ANY
 * IMPLIED WARRANTIES OF MERCHANTABILITY, NONINFRINGEMENT AND FITNESS FOR A
 * PARTICULAR PURPOSE.
 *
 * 4.2. IN NO EVENT SHALL INTEL HAVE ANY LIABILITY TO LICENSEE, ITS LICENSEES
 * OR ANY OTHER THIRD PARTY, FOR ANY LOST PROFITS, LOST DATA, LOSS OF USE OR
 * COSTS OF PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES, OR FOR ANY INDIRECT,
 * SPECIAL OR CONSEQUENTIAL DAMAGES ARISING OUT OF THIS AGREEMENT, UNDER ANY
 * CAUSE OF ACTION OR THEORY OF LIABILITY, AND IRRESPECTIVE OF WHETHER INTEL
 * HAS ADVANCE NOTICE OF THE POSSIBILITY OF SUCH DAMAGES.  THESE LIMITATIONS
 * SHALL APPLY NOTWITHSTANDING THE FAILURE OF THE ESSENTIAL PURPOSE OF ANY
 * LIMITED REMEDY.
 *
 * 4.3. Licensee shall not export, either directly or indirectly, any of this
 * software or system incorporating such software without first obtaining any
 * required license or other approval from the U. S. Department of Commerce or
 * any other agency or department of the United States Government.  In the
 * event Licensee exports any such software from the United States or
 * re-exports any such software from a foreign destination, Licensee shall
 * ensure that the distribution and export/re-export of the software is in
 * compliance with all laws, regulations, orders, or other restrictions of the
 * U.S. Export Administration Regulations. Licensee agrees that neither it nor
 * any of its subsidiaries will export/re-export any technical data, process,
 * software, or service, directly or indirectly, to any country for which the
 * United States government or any agency thereof requires an export license,
 * other governmental approval, or letter of assurance, without first obtaining
 * such license, approval or letter.
 *
 *****************************************************************************/

#define __NSACCESS_C__

#include "acpi.h"
#include "amlcode.h"
#include "acinterp.h"
#include "acnamesp.h"
#include "acdispat.h"


#define _COMPONENT          NAMESPACE
        MODULE_NAME         ("nsaccess");



/*******************************************************************************
 *
 * FUNCTION:    AcpiNsRootInitialize
 *
 * PARAMETERS:  None
 *
 * RETURN:      Status
 *
 * DESCRIPTION: Allocate and initialize the default root named objects
 *
 * MUTEX:       Locks namespace for entire execution
 *
 ******************************************************************************/

ACPI_STATUS
AcpiNsRootInitialize (void)
{
    ACPI_STATUS             Status = AE_OK;
    PREDEFINED_NAMES        *InitVal = NULL;
    ACPI_NAMED_OBJECT       *NewNameDesc;
    ACPI_OBJECT_INTERNAL    *ObjDesc;


    FUNCTION_TRACE ("NsRootInitialize");


    AcpiCmAcquireMutex (ACPI_MTX_NAMESPACE);

    /*
     * The global root ptr is initially NULL, so a non-NULL value indicates
     * that AcpiNsRootInitialize() has already been called; just return.
     */

    if (AcpiGbl_RootObject)
    {
        Status = AE_OK;
        goto UnlockAndExit;
    }


    /* 
     * Tell the rest of the subsystem that the root is initialized
     * (This is OK because the namespace is locked)
     */

    AcpiGbl_RootObject = &AcpiGbl_RootNamedObject;


    /* Enter the pre-defined names in the name table */

    DEBUG_PRINT (ACPI_INFO,
        ("Entering predefined name table entries into namespace\n"));

    for (InitVal = AcpiGbl_PreDefinedNames; InitVal->Name; InitVal++)
    {
        Status = AcpiNsLookup (NULL, InitVal->Name,
                                (OBJECT_TYPE_INTERNAL) InitVal->Type,
                                IMODE_LOAD_PASS2, NS_NO_UPSEARCH,
                                NULL, &NewNameDesc);

        if (ACPI_FAILURE (Status) ||
            (!NewNameDesc))
        {
            DEBUG_PRINT (ACPI_ERROR,
                ("Could not create predefined name %s, %s\n",
                InitVal->Name, AcpiCmFormatException (Status)));
        }

        /*
         * Name entered successfully.
         * If entry in PreDefinedNames[] specifies an
         * initial value, create the initial value.
         */

        if (InitVal->Val)
        {
            /*
             * Entry requests an initial value, allocate a
             * descriptor for it.
             */

            ObjDesc = AcpiCmCreateInternalObject (
                            (OBJECT_TYPE_INTERNAL) InitVal->Type);

            if (!ObjDesc)
            {
                Status = AE_NO_MEMORY;
                goto UnlockAndExit;
            }

            /*
             * Convert value string from table entry to
             * internal representation. Only types actually
             * used for initial values are implemented here.
             */

            switch (InitVal->Type)
            {

            case ACPI_TYPE_NUMBER:

                ObjDesc->Number.Value =
                        (UINT32) STRTOUL (InitVal->Val, NULL, 10);
                break;


            case ACPI_TYPE_STRING:

                ObjDesc->String.Length =
                        (UINT16) STRLEN (InitVal->Val);

                /*
                 * Allocate a buffer for the string.  All
                 * String.Pointers must be allocated buffers!
                 * (makes deletion simpler)
                 */
                ObjDesc->String.Pointer = AcpiCmAllocate (
                                                (ObjDesc->String.Length + 1));

                if (!ObjDesc->String.Pointer)
                {
                    REPORT_ERROR ("Initial value string"
                                    "allocation failure");

                    AcpiCmRemoveReference (ObjDesc);
                    Status = AE_NO_MEMORY;
                    goto UnlockAndExit;
                }

                STRCPY (ObjDesc->String.Pointer, InitVal->Val);
                break;


            case ACPI_TYPE_MUTEX:

                ObjDesc->Mutex.SyncLevel =
                        (UINT16) STRTOUL (InitVal->Val, NULL, 10);

                if (STRCMP (InitVal->Name, "_GL_") == 0)
                {
                    /*
                     * Create a counting semaphore for the
                     * global lock
                     */
                    Status = AcpiOsCreateSemaphore (ACPI_NO_UNIT_LIMIT,
                                            1, &ObjDesc->Mutex.Semaphore);

                    if (ACPI_FAILURE (Status))
                    {
                        goto UnlockAndExit;
                    }
                    /*
                     * We just created the mutex for the
                     * global lock, save it
                     */

                    AcpiGbl_GlobalLockSemaphore = ObjDesc->Mutex.Semaphore;
                }

                else
                {
                    /* Create a mutex */

                    Status = AcpiOsCreateSemaphore (1, 1,
                                        &ObjDesc->Mutex.Semaphore);

                    if (ACPI_FAILURE (Status))
                    {
                        goto UnlockAndExit;
                    }
                }

                /* TBD: [Restructure] These fields may be obsolete */

                ObjDesc->Mutex.LockCount = 0;
                ObjDesc->Mutex.ThreadId  = 0;
                break;


            default:
                REPORT_ERROR ("Unsupported initial type value");
                AcpiCmRemoveReference (ObjDesc);
                ObjDesc = NULL;
                continue;
            }

            /* Store pointer to value descriptor in the Named Object */

            AcpiNsAttachObject (NewNameDesc, ObjDesc,
                                ObjDesc->Common.Type);
        }
    }


UnlockAndExit:
    AcpiCmReleaseMutex (ACPI_MTX_NAMESPACE);
    return_ACPI_STATUS (Status);
}


/*******************************************************************************
 *
 * FUNCTION:    AcpiNsLookup
 *
 * PARAMETERS:  PrefixNameDesc  - Search scope if name is not fully qualified
 *              Pathname        - Search pathname, in internal format
 *                                (as represented in the AML stream)
 *              Type            - Type associated with name
 *              InterpreterMode - IMODE_LOAD_PASS2 => add name if not found
 *              Flags           - Flags describing the search restrictions
 *              WalkState       - Current state of the walk
 *              ReturnNameDesc  - Where the Named Object is placed (if found
 *                                or created successfully)
 *
 * RETURN:      Status
 *
 * DESCRIPTION: Find or enter the passed name in the name space.
 *              Log an error if name not found in Exec mode.
 *
 * MUTEX:       Assumes namespace is locked.
 *
 ******************************************************************************/

ACPI_STATUS
AcpiNsLookup (
    ACPI_GENERIC_STATE      *ScopeInfo,
    NATIVE_CHAR             *Pathname,
    OBJECT_TYPE_INTERNAL    Type,
    OPERATING_MODE          InterpreterMode,
    UINT32                  Flags,
    ACPI_WALK_STATE         *WalkState,
    ACPI_NAMED_OBJECT       **ReturnNameDesc)
{
    ACPI_STATUS             Status;
    ACPI_NAMED_OBJECT        *PrefixNameDesc;
    ACPI_NAMED_OBJECT       *CurrentNameDesc = NULL;
    ACPI_NAMED_OBJECT       *ScopeToPush = NULL;
    ACPI_NAMED_OBJECT       *ThisNameDesc = NULL;
    UINT32                  NumSegments;
    ACPI_NAME               SimpleName;
    BOOLEAN                 NullNamePath = FALSE;
    OBJECT_TYPE_INTERNAL    TypeToCheckFor;
    OBJECT_TYPE_INTERNAL    ThisSearchType;
    DEBUG_EXEC              (UINT32 i)


    FUNCTION_TRACE ("NsLookup");


    if (!ReturnNameDesc)
    {
        return_ACPI_STATUS (AE_BAD_PARAMETER);
    }


    AcpiGbl_NsLookupCount++;

    *ReturnNameDesc = ENTRY_NOT_FOUND;


    if (!AcpiGbl_RootObject)
    {
        return (AE_NO_NAMESPACE);
    }

    /*
     * Get the prefix scope.
     * A null scope means use the root scope
     */

    if ((!ScopeInfo) ||
        (!ScopeInfo->Scope.NameTable))
    {
        PrefixNameDesc = AcpiGbl_RootObject;
    }
    else
    {
        PrefixNameDesc = ScopeInfo->Scope.NameTable;
    }


    /*
     * This check is explicitly split provide relax the TypeToCheckFor
     * conditions for BankFieldDefn.  Originally, both BankFieldDefn and
     * DefFieldDefn caused TypeToCheckFor to be set to ACPI_TYPE_REGION,
     * but the BankFieldDefn may also check for a Field definition as well
     * as an OperationRegion.
     */

    if (INTERNAL_TYPE_DEF_FIELD_DEFN == Type)
    {
        /* DefFieldDefn defines fields in a Region */

        TypeToCheckFor = ACPI_TYPE_REGION;
    }

    else if (INTERNAL_TYPE_BANK_FIELD_DEFN == Type)
    {
        /* BankFieldDefn defines data fields in a Field Object */

        TypeToCheckFor = ACPI_TYPE_ANY;
    }

    else
    {
        TypeToCheckFor = Type;
    }



    /* TBD: [Restructure] - Move the pathname stuff into a new procedure */

    /* Examine the name pointer */

    if (!Pathname)
    {
        /*  8-12-98 ASL Grammar Update supports null NamePath   */

        NullNamePath = TRUE;
        NumSegments = 0;
        ThisNameDesc = AcpiGbl_RootObject;

        DEBUG_PRINT (TRACE_NAMES,
            ("NsLookup: Null Pathname (Zero segments),  Flags=%x\n", Flags));
    }

    else
    {
        /*
         * Valid name pointer (Internal name format)
         *
         * Check for prefixes.  As represented in the AML stream, a
         * Pathname consists of an optional scope prefix followed by
         * a segment part.
         *
         * If present, the scope prefix is either a RootPrefix (in
         * which case the name is fully qualified), or zero or more
         * ParentPrefixes (in which case the name's scope is relative
         * to the current scope).
         *
         * The segment part consists of either:
         *  - A single 4-byte name segment, or
         *  - A DualNamePrefix followed by two 4-byte name segments, or
         *  - A MultiNamePrefixOp, followed by a byte indicating the
         *    number of segments and the segments themselves.
         */

        if (*Pathname == AML_ROOT_PREFIX)
        {
            /* Pathname is fully qualified, look in root name table */

            CurrentNameDesc = AcpiGbl_RootObject;

            /* point to segment part */

            Pathname++;

            DEBUG_PRINT (TRACE_NAMES,
                ("NsLookup: Searching from root [%p]\n",
                CurrentNameDesc));

            /* Direct reference to root, "\" */

            if (!(*Pathname))
            {
                ThisNameDesc = AcpiGbl_RootObject;
                goto CheckForNewScopeAndExit;
            }
        }

        else
        {
            /* Pathname is relative to current scope, start there */

            CurrentNameDesc = PrefixNameDesc;

            DEBUG_PRINT (TRACE_NAMES,
                ("NsLookup: Searching relative to pfx scope [%p]\n",
                PrefixNameDesc));

            /*
             * Handle up-prefix (carat).  More than one prefix
             * is supported
             */

            while (*Pathname == AML_PARENT_PREFIX)
            {
                /* Point to segment part or next ParentPrefix */

                Pathname++;

                /*  Backup to the parent's scope  */

                ThisNameDesc = AcpiNsGetParentObject (CurrentNameDesc);
                if (!ThisNameDesc)
                {
                    /* Current scope has no parent scope */

                    REPORT_ERROR ("Too many parent prefixes (^) - reached root");

                    return_ACPI_STATUS (AE_NOT_FOUND);
                }

                CurrentNameDesc = ThisNameDesc;
            }
        }


        /*
         * Examine the name prefix opcode, if any,
         * to determine the number of segments
         */

        if (*Pathname == AML_DUAL_NAME_PREFIX)
        {
            NumSegments = 2;

            /* point to first segment */

            Pathname++;

            DEBUG_PRINT (TRACE_NAMES,
                ("NsLookup: Dual Pathname (2 segments, Flags=%X)\n", Flags));
        }

        else if (*Pathname == AML_MULTI_NAME_PREFIX_OP)
        {
            NumSegments = (UINT32)* (UINT8 *) ++Pathname;

            /* point to first segment */

            Pathname++;

            DEBUG_PRINT (TRACE_NAMES,
                ("NsLookup: Multi Pathname (%d Segments, Flags=%X) \n",
                NumSegments, Flags));
        }

        else
        {
            /*
             * No Dual or Multi prefix, hence there is only one
             * segment and Pathname is already pointing to it.
             */
            NumSegments = 1;

            DEBUG_PRINT (TRACE_NAMES,
                ("NsLookup: Simple Pathname (1 segment, Flags=%X)\n", Flags));
        }

#ifdef ACPI_DEBUG

        /* TBD: [Restructure] Make this a procedure */

        /* Debug only: print the entire name that we are about to lookup */

        DEBUG_PRINT (TRACE_NAMES, ("NsLookup: ["));

        for (i = 0; i < NumSegments; i++)
        {
            DEBUG_PRINT_RAW (TRACE_NAMES, ("%4.4s/", &Pathname[i * 4]));
        }
        DEBUG_PRINT_RAW (TRACE_NAMES, ("]\n"));
#endif
    }




    /*
     * Search namespace for each segment of the name.
     * Loop through and verify/add each name segment.
     */


    while (NumSegments-- && CurrentNameDesc)
    {
        /*
         * Search for the current name segment under the current
         * named object.  The Type is significant only at the last (topmost) 
         * level.  (We don't care about the types along the path, only
         * the type of the final target object.)
         */
        ThisSearchType = ACPI_TYPE_ANY;
        if (!NumSegments)
        {
            ThisSearchType = Type;
        }

        /* Pluck and ACPI name from the front of the pathname */

        MOVE_UNALIGNED32_TO_32 (&SimpleName, Pathname);

        /* Try to find the ACPI name */

        Status = AcpiNsSearchAndEnter (SimpleName, WalkState,
                                        CurrentNameDesc, InterpreterMode,
                                        ThisSearchType, Flags,
                                        &ThisNameDesc);

        if (ACPI_FAILURE (Status))
        {
            if (Status == AE_NOT_FOUND)
            {
                /* Name not found in ACPI namespace  */

                DEBUG_PRINT (TRACE_NAMES,
                    ("NsLookup: Name [%4.4s] not found in scope %X\n",
                    &SimpleName, CurrentNameDesc));
            }

            return_ACPI_STATUS (Status);
        }


        /*
         * If 1) This is the last segment (NumSegments == 0)
         *    2) and looking for a specific type
         *       (Not checking for TYPE_ANY)
         *    3) which is not a local type (TYPE_DEF_ANY)
         *    4) which is not a local type (TYPE_SCOPE)
         *    5) which is not a local type (TYPE_INDEX_FIELD_DEFN)
         *    6) and type of object is known (not TYPE_ANY)
         *    7) and object does not match request
         *
         * Then we have a type mismatch.  Just warn and ignore it.
         */
        if ((NumSegments        == 0)                               &&
            (TypeToCheckFor     != ACPI_TYPE_ANY)                   &&
            (TypeToCheckFor     != INTERNAL_TYPE_DEF_ANY)           &&
            (TypeToCheckFor     != INTERNAL_TYPE_SCOPE)             &&
            (TypeToCheckFor     != INTERNAL_TYPE_INDEX_FIELD_DEFN)  &&
            (ThisNameDesc->Type != ACPI_TYPE_ANY)                   &&
            (ThisNameDesc->Type != TypeToCheckFor))
        {
            /* Complain about a type mismatch */

            REPORT_WARNING ("Type mismatch");
            DEBUG_PRINT (ACPI_WARN,
                ("NsLookup: %4.4s, type 0x%X, checking for type 0x%X\n",
                &SimpleName, ThisNameDesc->Type, TypeToCheckFor));
        }

        /*
         * If this is the last name segment and we are not looking for a 
         * specific type, but the type of found object is known, use that type
         * to see if it opens a scope.
         */

        if ((0 == NumSegments) && (ACPI_TYPE_ANY == Type))
        {
            Type = ThisNameDesc->Type;
        }

        if ((NumSegments || AcpiNsOpensScope (Type)) &&
            (ThisNameDesc->Child == NULL))
        {
            /*
             * More segments or the type implies enclosed scope,
             * and the next scope has not been allocated.
             */

            DEBUG_PRINT (ACPI_INFO,
                ("NsLookup: Load mode=%d  ThisNameDesc=%x\n",
                InterpreterMode, ThisNameDesc));
        }

        CurrentNameDesc = ThisNameDesc;

        /* point to next name segment */

        Pathname += ACPI_NAME_SIZE;
    }


    /*
     * Always check if we need to open a new scope
     */

CheckForNewScopeAndExit:

    if (!(Flags & NS_DONT_OPEN_SCOPE) && (WalkState))
    {
        /*
         * If entry is a type which opens a scope,
         * push the new scope on the scope stack.
         */

        if (AcpiNsOpensScope (TypeToCheckFor))
        {
            /*  8-12-98 ASL Grammar Update supports null NamePath   */

            if (NullNamePath)
            {
                /* TBD: [Investigate] - is this the correct thing to do? */

                ScopeToPush = NULL;
            }
            else
            {
                ScopeToPush = ThisNameDesc;
            }

            Status = AcpiDsScopeStackPush (ScopeToPush, Type,
                                            WalkState);
            if (ACPI_FAILURE (Status))
            {
                return_ACPI_STATUS (Status);
            }

            DEBUG_PRINT (ACPI_INFO,
                ("NsLookup: Set global scope to %p\n", ScopeToPush));
        }
    }

    *ReturnNameDesc = ThisNameDesc;
    return_ACPI_STATUS (AE_OK);
}

