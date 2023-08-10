/*
 *  ======== package.xs ========
 */

/*
 *  ======== getLibs ========
 *  Contribute wolfSSL library.
 */
function getLibs(prog) 
{
    return ("lib/wolfssl.a" + prog.build.target.suffix);
}
