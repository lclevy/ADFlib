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
    BOOL ok = FALSE, true = TRUE;

    if (argc <= 1) return 1;
    adfEnvInitDefault();
    if ((dev = adfMountDev(argv[1], TRUE))) {
        if ((vol = adfMount(dev, 0, TRUE))) {
            /* use dir cache (enables the crash) */
            adfChgEnvProp(PR_USEDIRC, &true);
            /* read all directory entries (crash happens here) */
            list = adfGetRDirEnt(vol, vol->curDirPtr, TRUE);
            /* success! we didn't crash */
            ok = TRUE;
            if (list) adfFreeDirList(list);
            adfUnMount(vol);
        }
        adfUnMountDev(dev);
    }
    adfEnvCleanUp();
    return ok ? 0 : 1;
}
