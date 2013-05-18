/*
Copyright (C) 2012  Yonathan Yusim

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

dr_a1.c

Service Location Descriptor.

*/

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#if defined(HAVE_INTTYPES_H)
#include <inttypes.h>
#elif defined(HAVE_STDINT_H)
#include <stdint.h>
#endif

#include "../dvbpsi.h"
#include "../dvbpsi_private.h"
#include "../descriptor.h"

#include "dr_a1.h"


/*****************************************************************************
 * dvbpsi_DecodeServiceLocationDr
 *****************************************************************************/
dvbpsi_service_location_dr_t * dvbpsi_DecodeServiceLocationDr(
                                        dvbpsi_descriptor_t * p_descriptor)
{
  dvbpsi_service_location_dr_t * p_decoded;
  int i;
  uint8_t * buf = p_descriptor->p_data;

  /* Check the tag */
  if(p_descriptor->i_tag != 0xa1)
    return NULL;

  /* Don't decode twice */
  if(p_descriptor->p_decoded)
    return p_descriptor->p_decoded;

  /* Allocate memory */
  p_decoded =
        (dvbpsi_service_location_dr_t*)malloc(sizeof(dvbpsi_service_location_dr_t));
  if(!p_decoded) return NULL;

  memset(p_decoded, 0, sizeof(dvbpsi_service_location_dr_t));

  /* Decode data and check the length */
  if(p_descriptor->i_length < 3)
  {
    free(p_decoded);
    return NULL;
  }

  p_descriptor->p_decoded = (void*)p_decoded;

  p_decoded->i_PCR_PID = dvbpsi_get_bits(buf, 3, 13);
  p_decoded->i_number_elements = dvbpsi_get_bits(buf, 16, 8);

  buf = &p_descriptor->p_data[3];

  for (i=0; i<p_decoded->i_number_elements; i++)
  {
    dvbpsi_service_location_element_t * p_element =
        (dvbpsi_service_location_element_t*)malloc(sizeof(dvbpsi_service_location_element_t));

    if(!p_element) return NULL;

    memset(p_element, 0, sizeof(dvbpsi_service_location_element_t));

    p_element->i_stream_type = dvbpsi_get_bits(buf, 0, 8);
    p_element->i_elementary_PID = dvbpsi_get_bits(buf, 11, 13);
    memcpy(p_element->i_ISO_639_language_code, &buf[3], sizeof(p_element->i_ISO_639_language_code));

    if (p_decoded->p_first_element == NULL)
      p_decoded->p_first_element = p_element;
    else
    {
      dvbpsi_service_location_element_t* p_last_element = p_decoded->p_first_element;
      while (p_last_element->p_next != NULL)
          p_last_element = p_last_element->p_next;
        p_last_element->p_next = p_element;
    }

    buf += 6;
  }


  return p_decoded;
}

/*****************************************************************************
 * dvbpsi_FreeServiceLocationDr
 *****************************************************************************/
void dvbpsi_FreeServiceLocationDr(dvbpsi_service_location_dr_t * descriptor)
{
 dvbpsi_service_location_element_t* p_element = NULL;

 if (descriptor == NULL)
   return;

 p_element = descriptor->p_first_element;

 while(p_element)
 {
   dvbpsi_service_location_element_t* p_next = p_element->p_next;
   free(p_element);
   p_element = p_next;
 }
}

#if 0
/*****************************************************************************
 * dvbpsi_GenServiceDr
 *****************************************************************************/
dvbpsi_descriptor_t * dvbpsi_GenServiceDr(
                                        dvbpsi_service_location_dr_t * p_decoded,
                                        bool b_duplicate)
{
  /* Create the descriptor */
  dvbpsi_descriptor_t * p_descriptor =
        dvbpsi_NewDescriptor(0x48, 3 + p_decoded->i_service_location_name_length +
               p_decoded->i_service_location_provider_name_length , NULL);

  if(p_descriptor)
  {
    /* Encode data */
    p_descriptor->p_data[0] = p_decoded->i_service_type;
    p_descriptor->p_data[1] = p_decoded->i_service_provider_name_length;
    if(p_decoded->i_service_provider_name_length)
      memcpy(p_descriptor->p_data + 2,
             p_decoded->i_service_provider_name,
             p_decoded->i_service_provider_name_length);
    p_descriptor->p_data[2+p_decoded->i_service_provider_name_length] =
      p_decoded->i_service_name_length;
    if(p_decoded->i_service_name_length)
      memcpy(p_descriptor->p_data + 3 + p_decoded->i_service_provider_name_length,
             p_decoded->i_service_name,
             p_decoded->i_service_name_length);

    if(b_duplicate)
    {
      /* Duplicate decoded data */
      dvbpsi_service_dr_t * p_dup_decoded =
        (dvbpsi_service_dr_t*)malloc(sizeof(dvbpsi_service_dr_t));
      if(p_dup_decoded)
        memcpy(p_dup_decoded, p_decoded, sizeof(dvbpsi_service_dr_t));

      p_descriptor->p_decoded = (void*)p_dup_decoded;
    }
  }

  return p_descriptor;
}
#endif