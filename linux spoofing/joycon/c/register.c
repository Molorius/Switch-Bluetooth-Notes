
#include "register.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/sdp.h>
#include <bluetooth/sdp_lib.h>

void register_name(char* name) {
    char* buf;

    buf = malloc(30 + strlen(name)); // allocate space
    system("hciconfig hci0 up"); // bring up the device, if not already
    sprintf(buf, "hciconfig hci0 name \"%s\"", name); // prepare the first command
    system(buf); // set the name
    system("hciconfig hci0 down"); // bring down the device
    system("hciconfig hci0 up"); // bring it back up
    free(buf); // free memory
}

// largely based on add_hid_wiimote
// from https://android.googlesource.com/platform/external/bluez.git/+/02e95fd30e347523c759e53ba5e6d7c9bd00f7d0/utils/tools/sdptool.c
static int _register_hid(sdp_session_t* session, char* name) {
    sdp_record_t record;
    uuid_t hid_uuid, l2cap_uuid, hidp_uuid, root_uuid;
    sdp_list_t *svclass_id, *l2cap_id, *hidp_id, *apseq, *aproto, *root, *langs, *pfseq;
    sdp_data_t *psm, *lang_lst, *lang_lst2, *hid_spec_lst, *hid_spec_lst2;
    sdp_lang_attr_t base_lang;
    sdp_profile_desc_t profile[1];
    uint8_t dtd = SDP_UINT16;
    uint8_t dtd2 = SDP_UINT8;
    uint8_t dtd_data = SDP_TEXT_STR8;
    void *dtds[2];
    void *values[2];
    void *dtds2[2];
	void *values2[2];
    int leng[2];
    uint8_t hid_spec_type = 0x22;
    
    uint16_t hid_attr_lang[] = { 0x409, 0x100 };
    uint16_t ctrl = 0x11;
    uint16_t intr = 0x13;
    uint16_t parser_version = 0x0111;
    uint8_t subclass = 0x08;
    uint8_t countrycode = 0x21;
    bool virtualcable = 1;
    bool reconnectinitiate = 1;
    bool batterypower = 1;
    bool remotewakeup = 1;
    uint16_t supervisiontimeout = 0xc80;
    bool normallyconnectable = 0;
    bool bootdevice = 0;

    const uint8_t hid_spec[] = { 
        0x05, 0x01, 0x09, 0x05, 0xa1, 0x01, 0x06, 0x01, 
        0xff, 0x85, 0x21, 0x09, 0x21, 0x75, 0x08, 0x95, 
        0x30, 0x81, 0x02, 0x85, 0x30, 0x09, 0x30, 0x75, 
        0x08, 0x95, 0x30, 0x81, 0x02, 0x85, 0x31, 0x09, 
        0x31, 0x75, 0x08, 0x96, 0x69, 0x01, 0x81, 0x02, 
        0x85, 0x32, 0x09, 0x32, 0x75, 0x08, 0x96, 0x69, 
        0x01, 0x81, 0x02, 0x85, 0x33, 0x09, 0x33, 0x75, 
        0x08, 0x96, 0x69, 0x01, 0x81, 0x02, 0x85, 0x3f, 
        0x05, 0x09, 0x19, 0x01, 0x29, 0x10, 0x15, 0x00, 
        0x25, 0x01, 0x75, 0x01, 0x95, 0x10, 0x81, 0x02, 
        0x05, 0x01, 0x09, 0x39, 0x15, 0x00, 0x25, 0x07, 
        0x75, 0x04, 0x95, 0x01, 0x81, 0x42, 0x05, 0x09, 
        0x75, 0x04, 0x95, 0x01, 0x81, 0x01, 0x05, 0x01, 
        0x09, 0x30, 0x09, 0x31, 0x09, 0x33, 0x09, 0x34, 
        0x16, 0x00, 0x00, 0x27, 0xff, 0xff, 0x00, 0x00, 
        0x75, 0x10, 0x95, 0x04, 0x81, 0x02, 0x06, 0x01, 
        0xff, 0x85, 0x01, 0x09, 0x01, 0x75, 0x08, 0x95, 
        0x30, 0x91, 0x02, 0x85, 0x10, 0x09, 0x10, 0x75, 
        0x08, 0x95, 0x30, 0x91, 0x02, 0x85, 0x11, 0x09, 
        0x11, 0x75, 0x08, 0x95, 0x30, 0x91, 0x02, 0x85, 
        0x12, 0x09, 0x12, 0x75, 0x08, 0x95, 0x30, 0x91, 
        0x02, 0xc0, 0x00 };

    memset(&record, 0, sizeof(sdp_record_t));

    // this follows the structure in the browse/tree.txt

    // Attribute Identifier : 0x0 - ServiceRecordHandle
    //record.handle = 0x10000;
    //record.handle = -1;
    record.handle = 0xffffffff; // pick any!


    // Attribute Identifier : 0x1 - ServiceClassIDList
    sdp_uuid16_create(&hid_uuid, HID_SVCLASS_ID); // HID_SVCLASS_ID = 0x1124
    svclass_id = sdp_list_append(NULL, &hid_uuid);
    sdp_set_service_classes(&record, svclass_id);

    //sdp_list_free(svclass_id, 0);

    // Attribute Identifier : 0x4 - ProtocolDescriptorList
    sdp_uuid16_create(&l2cap_uuid, L2CAP_UUID);
    l2cap_id = sdp_list_append(0, &l2cap_uuid);
    psm = sdp_data_alloc(SDP_UINT16, &ctrl);
    l2cap_id = sdp_list_append(l2cap_id, psm);
    apseq = sdp_list_append(0, l2cap_id);

    sdp_uuid16_create(&hidp_uuid, HIDP_UUID);
    hidp_id = sdp_list_append(0, &hidp_uuid);
    apseq = sdp_list_append(apseq, hidp_id);

    aproto = sdp_list_append(0, apseq);
    sdp_set_access_protos(&record, aproto);

    /*
    sdp_list_free(l2cap_id, 0);
    sdp_data_free(psm);
    sdp_list_free(l2cap_id, 0);
    sdp_list_free(apseq, 0);
    sdp_list_free(hidp_id, 0);
    sdp_list_free(aproto, 0);
    */

    // Attribute Identifier : 0x5 - BrowseGroupList
    sdp_uuid16_create(&root_uuid, PUBLIC_BROWSE_GROUP);
	root = sdp_list_append(NULL, &root_uuid);
	sdp_set_browse_groups(&record, root);

    // Attribute Identifier : 0x6 - LanguageBaseAttributeIDList
    base_lang.code_ISO639 = 0x656e;
	base_lang.encoding = 0x6a;
	base_lang.base_offset = SDP_PRIMARY_LANG_BASE; // 0x100
	langs = sdp_list_append(0, &base_lang);
	sdp_set_lang_attr(&record, langs);

	//sdp_list_free(langs, 0);

    // Attribute Identifier : 0x9 - BluetoothProfileDescriptorList
    sdp_uuid16_create(&profile[0].uuid, HID_PROFILE_ID);
    profile[0].version = 0x101;
    pfseq = sdp_list_append(NULL, profile);
    sdp_set_profile_descs(&record, pfseq);

    // Attribute Identifier : 0xd - AdditionalProtocolDescriptorLists
    l2cap_id = sdp_list_append(0, &l2cap_uuid);
    psm = sdp_data_alloc(SDP_UINT16, &intr);
    l2cap_id = sdp_list_append(l2cap_id, psm);
    apseq = sdp_list_append(0, l2cap_id);
    sdp_uuid16_create(&hidp_uuid, HIDP_UUID);
    hidp_id  = sdp_list_append(0, &hidp_uuid);
    apseq = sdp_list_append(apseq, hidp_id);
    aproto = sdp_list_append(0, apseq);
    sdp_set_add_access_protos(&record, aproto);

    // Attribute Identifier : 0x100
    // Attribute Identifier : 0x101
    // Attribute Identifier : 0x102
    sdp_set_info_attr(&record, "Wireless Gamepad", "Nintendo", "Gamepad");

    // Attribute Identifier : 0x201 - 0x205
    sdp_attr_add_new(&record, SDP_ATTR_HID_PARSER_VERSION, SDP_UINT16, &parser_version);
    sdp_attr_add_new(&record, SDP_ATTR_HID_DEVICE_SUBCLASS, SDP_UINT8, &subclass);
    sdp_attr_add_new(&record, SDP_ATTR_HID_COUNTRY_CODE, SDP_UINT8, &countrycode);
    sdp_attr_add_new(&record, SDP_ATTR_HID_VIRTUAL_CABLE, SDP_BOOL, &virtualcable);
    sdp_attr_add_new(&record, SDP_ATTR_HID_RECONNECT_INITIATE, SDP_BOOL, &reconnectinitiate);

    // Attribute Identifier : 0x206 - DescriptorList
    dtds[0] = &dtd2;
    values[0] = &hid_spec_type;
    dtds[1] = &dtd_data;
    values[1] = (uint8_t*) hid_spec;
    leng[0] = 0;
    leng[1] = sizeof(hid_spec);
    hid_spec_lst = sdp_seq_alloc_with_length(dtds, values, leng, 2);
	hid_spec_lst2 = sdp_data_alloc(SDP_SEQ8, hid_spec_lst);
	sdp_attr_add(&record, SDP_ATTR_HID_DESCRIPTOR_LIST, hid_spec_lst2);
    
    // Attribute Identifier : 0x207 - LangIDBaseList
    for (int i = 0; i < sizeof(hid_attr_lang) / 2; i++) {
		dtds2[i] = &dtd;
		values2[i] = &hid_attr_lang[i];
	}
	lang_lst = sdp_seq_alloc(dtds2, values2, sizeof(hid_attr_lang) / 2);
	lang_lst2 = sdp_data_alloc(SDP_SEQ8, lang_lst);
	sdp_attr_add(&record, SDP_ATTR_HID_LANG_ID_BASE_LIST, lang_lst2);

    // Attribute Identifier : 0x209 - 0x20e
    sdp_attr_add_new(&record, SDP_ATTR_HID_BATTERY_POWER, SDP_BOOL, &batterypower);
    sdp_attr_add_new(&record, SDP_ATTR_HID_REMOTE_WAKEUP, SDP_BOOL, &remotewakeup);
    sdp_attr_add_new(&record, SDP_ATTR_HID_SUPERVISION_TIMEOUT, SDP_UINT16, &supervisiontimeout);
    sdp_attr_add_new(&record, SDP_ATTR_HID_NORMALLY_CONNECTABLE, SDP_BOOL, &normallyconnectable);
    sdp_attr_add_new(&record, SDP_ATTR_HID_BOOT_DEVICE, SDP_BOOL, &bootdevice);

    if(sdp_record_register(session, &record, SDP_RECORD_PERSIST) < 0) {
        printf("Could not register HID\n");
        return -1;
    }
    printf("Registered HID\n");
    return 0;
}

void register_records(char* name) {
    sdp_session_t* session = 0;

    session = sdp_connect(BDADDR_ANY, BDADDR_LOCAL, SDP_RETRY_IF_BUSY);
    _register_hid(session, name);
}
