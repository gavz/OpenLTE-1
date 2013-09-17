/*******************************************************************************

    Copyright 2013 Ben Wojtowicz

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*******************************************************************************

    File: liblte_mac.cc

    Description: Contains all the implementations for the LTE Medium Access
                 Control Layer library.

    Revision History
    ----------    -------------    --------------------------------------------
    07/21/2013    Ben Wojtowicz    Created file.

*******************************************************************************/

/*******************************************************************************
                              INCLUDES
*******************************************************************************/

#include "liblte_mac.h"

/*******************************************************************************
                              DEFINES
*******************************************************************************/


/*******************************************************************************
                              TYPEDEFS
*******************************************************************************/


/*******************************************************************************
                              GLOBAL VARIABLES
*******************************************************************************/


/*******************************************************************************
                              LOCAL FUNCTION PROTOTYPES
*******************************************************************************/

/*********************************************************************
    Name: mac_value_2_bits

    Description: Converts a value to a bit string
*********************************************************************/
void mac_value_2_bits(uint32   value,
                      uint8  **bits,
                      uint32   N_bits);

/*********************************************************************
    Name: mac_bits_2_value

    Description: Converts a bit string to a value
*********************************************************************/
uint32 mac_bits_2_value(uint8  **bits,
                        uint32   N_bits);

/*******************************************************************************
                              PDU FUNCTIONS
*******************************************************************************/

/*********************************************************************
    PDU Name: DL-SCH and UL-SCH

    Description: PDU containing a MAC header, zero or more MAC SDUs,
                 and zero or more MAC control elements

    Document Reference: 36.321 v10.2.0 Section 6.1.2
*********************************************************************/
// FIXME

/*********************************************************************
    PDU Name: Transparent

    Description: PDU containing a MAC SDU

    Document Reference: 36.321 v10.2.0 Section 6.1.4
*********************************************************************/
// FIXME

/*********************************************************************
    PDU Name: Random Access Response

    Description: PDU containing a MAC header and zero or more MAC
                 Random Access Responses

    Document Reference: 36.321 v10.2.0 Section 6.1.5

    Notes: Currently only supports 1 RAR per PDU
*********************************************************************/
LIBLTE_ERROR_ENUM liblte_mac_pack_random_access_response_pdu(LIBLTE_MAC_RAR_STRUCT *rar,
                                                             LIBLTE_MSG_STRUCT     *pdu)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *pdu_ptr = pdu->msg;

    if(rar != NULL &&
       pdu != NULL)
    {
        if(LIBLTE_MAC_RAR_HEADER_TYPE_BI == rar->hdr_type)
        {
            // Pack Header
            mac_value_2_bits(0,             &pdu_ptr, 1); // E
            mac_value_2_bits(rar->hdr_type, &pdu_ptr, 1);
            mac_value_2_bits(0,             &pdu_ptr, 2); // R
            mac_value_2_bits(rar->BI,       &pdu_ptr, 4);

            err = LIBLTE_SUCCESS;
        }else if(LIBLTE_MAC_RAR_HEADER_TYPE_RAPID == rar->hdr_type){
            // Pack Header
            mac_value_2_bits(0,             &pdu_ptr, 1); // E
            mac_value_2_bits(rar->hdr_type, &pdu_ptr, 1);
            mac_value_2_bits(rar->RAPID,    &pdu_ptr, 6);

            // Pack RAR
            mac_value_2_bits(0,                   &pdu_ptr, 1); // R
            mac_value_2_bits(rar->timing_adv_cmd, &pdu_ptr, 11);
            mac_value_2_bits(rar->hopping_flag,   &pdu_ptr, 1);
            mac_value_2_bits(rar->rba,            &pdu_ptr, 10); // FIXME
            mac_value_2_bits(rar->mcs,            &pdu_ptr, 4); // FIXME
            mac_value_2_bits(rar->tpc_command,    &pdu_ptr, 3);
            mac_value_2_bits(rar->ul_delay,       &pdu_ptr, 1);
            mac_value_2_bits(rar->csi_req,        &pdu_ptr, 1);
            mac_value_2_bits(rar->temp_crnti,     &pdu_ptr, 16);

            err = LIBLTE_SUCCESS;
        }

        pdu->N_bits = pdu_ptr - pdu->msg;
    }

    return(err);
}
LIBLTE_ERROR_ENUM liblte_mac_unpack_random_access_response_pdu(LIBLTE_MSG_STRUCT     *pdu,
                                                               LIBLTE_MAC_RAR_STRUCT *rar)
{
    LIBLTE_ERROR_ENUM  err     = LIBLTE_ERROR_INVALID_INPUTS;
    uint8             *pdu_ptr = pdu->msg;

    if(pdu != NULL &&
       rar != NULL)
    {
        mac_bits_2_value(&pdu_ptr, 1); // E
        rar->hdr_type = (LIBLTE_MAC_RAR_HEADER_TYPE_ENUM)mac_bits_2_value(&pdu_ptr, 1);
        if(LIBLTE_MAC_RAR_HEADER_TYPE_BI == rar->hdr_type)
        {
            mac_bits_2_value(&pdu_ptr, 2); // R
            rar->BI = mac_bits_2_value(&pdu_ptr, 4);

            err = LIBLTE_SUCCESS;
        }else if(LIBLTE_MAC_RAR_HEADER_TYPE_RAPID == rar->hdr_type){
            // Unpack header
            rar->RAPID = mac_bits_2_value(&pdu_ptr, 6);

            // Unpack RAR
            mac_bits_2_value(&pdu_ptr, 1); // R
            rar->timing_adv_cmd = mac_bits_2_value(&pdu_ptr, 11);
            rar->hopping_flag   = (LIBLTE_MAC_RAR_HOPPING_ENUM)mac_bits_2_value(&pdu_ptr, 1);
            rar->rba            = mac_bits_2_value(&pdu_ptr, 10); // FIXME
            rar->mcs            = mac_bits_2_value(&pdu_ptr, 4); // FIXME
            rar->tpc_command    = (LIBLTE_MAC_RAR_TPC_COMMAND_ENUM)mac_bits_2_value(&pdu_ptr, 3);
            rar->ul_delay       = (LIBLTE_MAC_RAR_UL_DELAY_ENUM)mac_bits_2_value(&pdu_ptr, 1);
            rar->csi_req        = (LIBLTE_MAC_RAR_CSI_REQ_ENUM)mac_bits_2_value(&pdu_ptr, 1);
            rar->temp_crnti     = mac_bits_2_value(&pdu_ptr, 16);

            err = LIBLTE_SUCCESS;
        }
    }

    return(err);
}

/*******************************************************************************
                              LOCAL FUNCTIONS
*******************************************************************************/

/*********************************************************************
    Name: mac_value_2_bits

    Description: Converts a value to a bit string
*********************************************************************/
void mac_value_2_bits(uint32   value,
                      uint8  **bits,
                      uint32   N_bits)
{
    uint32 i;

    for(i=0; i<N_bits; i++)
    {
        (*bits)[i] = (value >> (N_bits-i-1)) & 0x1;
    }
    *bits += N_bits;
}

/*********************************************************************
    Name: mac_bits_2_value

    Description: Converts a bit string to a value
*********************************************************************/
uint32 mac_bits_2_value(uint8  **bits,
                        uint32   N_bits)
{
    uint32 value = 0;
    uint32 i;

    for(i=0; i<N_bits; i++)
    {
        value |= (*bits)[i] << (N_bits-i-1);
    }
    *bits += N_bits;

    return(value);
}