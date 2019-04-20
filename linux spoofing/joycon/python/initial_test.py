#!/usr/bin/env python3

import dbus

def advertise_service(sdp_record_xml):
    bus = dbus.SystemBus()
    manager = dbus.Interface(bus.get_object("org.bluez", "/"), "org.bluez.Manager")
    #adapter_path = manager.FindAdapter(self.device_id)
    adapter_path = manager.DefaultAdapter()
    service = dbus.Interface(bus.get_object("org.bluez", adapter_path),
                             "org.bluez.Service")
    service.AddRecord(sdp_record_xml)

def advertise_service2(sdp_record_xml):
    bus = dbus.SystemBus()
    manager = dbus.Interface(bus.get_object("org.bluez", "/"), "org.freedesktop.DBus.ObjectManager")
    objects = manager.GetManagedObjects()
    for path, ifaces in objects.items():
        adapter = ifaces.get("org.bluez.Adapter1")
        if adapter is None:
            continue
        obj = bus.get_object("org.bluez", path)
        adapter = dbus.Interface(obj, "org.bluez.Adapter1").object_path
        break
    print(adapter)
    service = dbus.Interface(bus.get_object("org.bluez", adapter), "org.bluez.Service")

    with open(sdp_record_xml, "r") as f:
        record = f.read()
    service.AddRecord(record)

advertise_service2("pro controller 1.xml")