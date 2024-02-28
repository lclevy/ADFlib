#include <stdio.h>
#include "adflib.h"

/* verifies that Debian bug 862740 is fixed
 * https://bugs.debian.org/cgi-bin/bugreport.cgi?bug=862740
 * Usage: ./cache_crash ../Dumps/cache_crash.adf
 */
int main(int argc, char *argv[]) {
    struct AdfDevice *dev;
    struct AdfVolume *vol;
    struct AdfList *list;
    bool ok = false;

    if (argc <= 1) return 1;
    adfEnvInitDefault();
    adfEnvSetProperty ( ADF_PR_IGNORE_CHECKSUM_ERRORS, true );
    if ((dev = adfDevOpen(argv[1], ADF_ACCESS_MODE_READONLY))) {
        if (adfDevMount(dev) == RC_OK) {
            if ((vol = adfVolMount(dev, 0, ADF_ACCESS_MODE_READONLY))) {
                /* use dir cache (enables the crash) */
                adfEnvSetProperty(ADF_PR_USEDIRC, true);
                /* read all directory entries (crash happens here) */
                list = adfGetRDirEnt(vol, vol->curDirPtr, true);
                /* success! we didn't crash */
                ok = true;
                if (list) adfFreeDirList(list);
                adfVolUnMount(vol);
            }
            adfDevUnMount(dev);
        }
        adfDevClose(dev);
    }
    adfEnvCleanUp();
    return ok ? 0 : 1;
}
