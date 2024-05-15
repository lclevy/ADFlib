## Past security bugs

Please note that several security issues/bugs has been found in the older (0.7.x)
versions of the ADFlib:
- `CVE-2016-1243` and `CVE-2016-1244`, fixed in
[8e973d7](https://github.com/lclevy/ADFlib/commit/8e973d7b894552c3a3de0ccd2d1e9cb0b8e618dd)),
(found in Debian version `unadf/0.7.11a-3`, fixed in versions `unadf/0.7.11a-4`,
`unadf/0.7.11a-3+deb8u1`). See https://bugs.debian.org/cgi-bin/bugreport.cgi?bug=838248
- Stuart Caie fixed arbitrary directory traversal in
[4ce14b2](https://github.com/lclevy/ADFlib/commit/4ce14b2a8b6db84954cf9705459eafebabecf3e4)
lines 450-455

**Please update to the latest released version where these,
as well as many other things, are fixed.**
