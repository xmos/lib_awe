#include "descriptor_defs.h"


#define HID_INTERFACE_BYTES   ( 9 + 9 + 7 * 2 )
#define HID_INTERFACES_A1     1


#define HID_INTERFACE_DESCRIPTOR_LENGTH         ( 0x09 )    /* Size of descriptor in Bytes */
#define HID_INTERFACE_DESCRIPTOR_TYPE           ( 0x04 )    /* Interface 0x04 */
#define HID_INTERFACE_ALTERNATE_SETTING         ( 0x00 )    /* Value used alternate interfaces using SetInterface Request */
#define HID_INTERFACE_NUMBER_OF_ENDPOINTS       ( 0x02 )    /* Number of endpoitns for this interface (excluding 0) */
#define HID_INTERFACE_CLASS                     ( 0x03 )
#define HID_INTERFACE_SUBCLASS                  ( 0x00 )    /* No boot device */
#define HID_INTERFACE_PROTOCOL                  ( 0x00 )
#define HID_INTERFACE_STRING_DESCRIPTOR_INDEX   ( 0x00 )

#if (AUDIO_CLASS == 1)

    /* HID interface descriptor */
    HID_INTERFACE_DESCRIPTOR_LENGTH,        /* 0  bLength */
    HID_INTERFACE_DESCRIPTOR_TYPE,          /* 1  bDescriptorType */
    INTERFACE_NUMBER_HID,                   /* 2  bInterfaceNumber : Number of interface */
    HID_INTERFACE_ALTERNATE_SETTING,        /* 3  bAlternateSetting */
    HID_INTERFACE_NUMBER_OF_ENDPOINTS,      /* 4: bNumEndpoints */
    HID_INTERFACE_CLASS,                    /* 5: bInterfaceClass */
    HID_INTERFACE_SUBCLASS,                 /* 6: bInterfaceSubClass */
    HID_INTERFACE_PROTOCOL,                 /* 7: bInterfaceProtocol*/
    HID_INTERFACE_STRING_DESCRIPTOR_INDEX,  /* 8  iInterface */

#elif (AUDIO_CLASS == 2)

    .HID_Interface =
    {
        /* HID interface descriptor */
        .bLength            = sizeof(USB_Descriptor_Interface_t),
        .bDescriptorType    = HID_INTERFACE_DESCRIPTOR_TYPE,
        .bInterfaceNumber   = INTERFACE_NUMBER_HID,
        .bAlternateSetting  = HID_INTERFACE_ALTERNATE_SETTING,
        .bNumEndpoints      = HID_INTERFACE_NUMBER_OF_ENDPOINTS,
        .bInterfaceClass    = HID_INTERFACE_CLASS,
        .bInterfaceSubClass = HID_INTERFACE_SUBCLASS,
        .bInterfaceProtocol = HID_INTERFACE_PROTOCOL,
        .iInterface         = HID_INTERFACE_STRING_DESCRIPTOR_INDEX,
    },

#else
    #error "Unknown Audio Class"
#endif

#define HID_DESCRIPTOR_LENGTH_0    ( 0x09 ) /* Size of descriptor in Bytes */
#define HID_DESCRIPTOR_TYPE_0      ( 0x21 ) /* HID 0x21 */
#define HID_BCD_VERSION_LO         ( 0x10 ) /* HID class specification release */
#define HID_BCD_VERSION_HI         ( 0x01 )
#define HID_COUNTRY_CODE           ( 0x00 ) /* Country code of localized hardware */
#define HID_NUM_DESCRIPTORS        ( 0x01 ) /* Number of class descriptors */
#define HID_DESCRIPTOR_TYPE_1      ( 0x22 ) /* Type of 1st class descriptor, Report 0x22 */
#define HID_DESCRIPTOR_LENGTH_1_LO ( 32 )
#define HID_DESCRIPTOR_LENGTH_1_HI ( 0 )


#if (AUDIO_CLASS == 1)

    /* HID descriptor */
    HID_DESCRIPTOR_LENGTH_0,     /* 0  bLength */
    HID_DESCRIPTOR_TYPE_0,       /* 1  bDescriptorType (HID) */
    HID_BCD_VERSION_LO,          /* 2  bcdHID */
    HID_BCD_VERSION_HI,          /* 3  bcdHID */
    HID_COUNTRY_CODE,            /* 4  bCountryCode */
    HID_NUM_DESCRIPTORS,         /* 5  bNumDescriptors */
    HID_DESCRIPTOR_TYPE_1,       /* 6  bDescriptorType[0] */
    HID_DESCRIPTOR_LENGTH_1_LO,  /* 7  wDescriptorLength[0] */
    HID_DESCRIPTOR_LENGTH_1_HI,  /* 8  wDescriptorLength[0] */

#elif (AUDIO_CLASS == 2)

    .HID_Descriptor =
    {
        /* HID descriptor */
        .bLength            = sizeof(USB_HID_Descriptor_t),
        .bDescriptorType0   = HID_DESCRIPTOR_TYPE_0,
        .bcdHID             =
        {
                              HID_BCD_VERSION_LO,
                              HID_BCD_VERSION_HI,
        },
        .bCountryCode       = HID_COUNTRY_CODE,
        .bNumDescriptors    = HID_NUM_DESCRIPTORS,
        .bDescriptorType1   = HID_DESCRIPTOR_TYPE_1,
        .wDescriptorLength1 =
        {
                              HID_DESCRIPTOR_LENGTH_1_LO,
                              HID_DESCRIPTOR_LENGTH_1_HI,
        },
    },

#else
    #error "Unknown Audio Class"
#endif

#define HID_ENDPOINT_DESCRIPTOR_LENGTH          ( 0x07 )    /* Size of descriptor in Bytes */
#define HID_ENDPOINT_DESCRIPTOR_TYPE            ( 0x05 )    /* Endpoint 0x05 */
#define HID_ENDPOINT_ATTRIBUTES                 ( 0x03 )    /* Interrupt */
#define HID_ENDPOINT_DESCRIPTOR_PACKET_SIZE_LO  ( 0x40 )
#define HID_ENDPOINT_DESCRIPTOR_PACKET_SIZE_HI  ( 0x00 )

#if (AUDIO_CLASS == 1)

    /* HID Endpoint descriptor (IN) */
    HID_ENDPOINT_DESCRIPTOR_LENGTH,         /* 0  bLength */
    HID_ENDPOINT_DESCRIPTOR_TYPE,           /* 1  bDescriptorType */
    ENDPOINT_ADDRESS_IN_HID,                /* 2  bEndpointAddress  */
    HID_ENDPOINT_ATTRIBUTES,                /* 3  bmAttributes (INTERRUPT) */
    HID_ENDPOINT_DESCRIPTOR_PACKET_SIZE_LO, /* 4  wMaxPacketSize */
    HID_ENDPOINT_DESCRIPTOR_PACKET_SIZE_HI, /* 5  wMaxPacketSize */
    ENDPOINT_INT_INTERVAL_IN_HID,           /* 6  bInterval */

    /* HID Endpoint descriptor (OUT) */
    HID_ENDPOINT_DESCRIPTOR_LENGTH,         /* 0  bLength */
    HID_ENDPOINT_DESCRIPTOR_TYPE,           /* 1  bDescriptorType */
    ENDPOINT_ADDRESS_OUT_HID,                /* 2  bEndpointAddress  */
    HID_ENDPOINT_ATTRIBUTES,                /* 3  bmAttributes (INTERRUPT) */
    HID_ENDPOINT_DESCRIPTOR_PACKET_SIZE_LO, /* 4  wMaxPacketSize */
    HID_ENDPOINT_DESCRIPTOR_PACKET_SIZE_HI, /* 5  wMaxPacketSize */
    ENDPOINT_INT_INTERVAL_OUT_HID,           /* 6  bInterval */

#elif (AUDIO_CLASS == 2)

    .HID_In_Endpoint =
    {
        /* Endpoint descriptor (IN) */
        .bLength            = sizeof(USB_Descriptor_Endpoint_t),
        .bDescriptorType    = HID_ENDPOINT_DESCRIPTOR_TYPE,
        .bEndpointAddress   = ENDPOINT_ADDRESS_IN_HID,
        .bmAttributes       = HID_ENDPOINT_ATTRIBUTES,
        .wMaxPacketSize     = HID_ENDPOINT_DESCRIPTOR_PACKET_SIZE_LO,
        .bInterval          = ENDPOINT_INT_INTERVAL_IN_HID,
    },

    .HID_Out_Endpoint =
    {
        /* Endpoint descriptor (OUT) */
        .bLength            = sizeof(USB_Descriptor_Endpoint_t),
        .bDescriptorType    = HID_ENDPOINT_DESCRIPTOR_TYPE,
        .bEndpointAddress   = ENDPOINT_ADDRESS_OUT_HID,
        .bmAttributes       = HID_ENDPOINT_ATTRIBUTES,
        .wMaxPacketSize     = HID_ENDPOINT_DESCRIPTOR_PACKET_SIZE_LO,
        .bInterval          = ENDPOINT_INT_INTERVAL_OUT_HID,
    },

#else
    #error "Unknown Audio Class"
#endif

