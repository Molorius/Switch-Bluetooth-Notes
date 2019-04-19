
[controller]/browse were taken with 'sdptool browse' and a corresponding option (raw.txt used --raw option)
[controller]/records were taken with 'sdptool records' and a corresponding option (xml.txt used --xml option)

All of these were taken with a device named "Nintendo Switch" so the controller assumed it was a Switch.
The device was running kernel 4.14.98.

Many of these say "Failed to connect to SDP server on [mac address]: Connection refused".
I assume this is a shortcoming of the current Linux bluetooth code.
However, this only appears with 'browse'