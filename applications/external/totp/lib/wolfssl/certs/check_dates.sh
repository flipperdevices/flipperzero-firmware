#!/bin/sh

# Whether a certificate or CRLs needs updating
expired=0
# Default to checking expiry within 6 months
offset="+6 months"

# First command line argument is the new expiry time
if [ "$1" != "" ]
then
    offset=$1
fi

# Certificates that are expired and are intentionally or irrelevantly so.
exp_expired="\
/test/crit-cert.pem \
/test/expired/expired-cert.pem \
/test/expired/expired-ca.pem \
/test/expired/expired-cert.der \
/test/expired/expired-ca.der \
/certeccrsa.pem \
/certeccrsa.der
"

# Files that are not certificates or CRLs put get matched anyway
ignore="\
/test/cert-ext-ns.der \
/rsa3072.der \
/rsa2048.der \
/1024/rsa1024.der \
"

# Get the date offset from now - earliest expiry - in seconds
earliest=`date -d "$offset" +%s`

# Compare the date with earliest allowed expiry.
#
# $1  Name of file being checked.
# $2  Expiry date in file (notAfter or nextUpdate).
check_expiry() {
    # Convert date to a number of seconds
    expiry=`date -d "$2" +%s`

    # Check expiry is not too soon
    if [ $expiry -lt $earliest ]
    then
        # Reset result
        result=expired
        # Ignore files that are expected to be expired
        for exp in $exp_expired
        do
            case $1 in
            *$exp)
                result=ignore
                break
                ;;
            esac
        done
        # Report any unexpected expiries
        if [ "$result" = "expired" ]
        then
            echo "$1 expires at:"
            echo "    '$2' (< $offset)"
            expired=1
        fi
    fi
}

# Check file expiry.
#
# The file is of any format.
# Try to guess from name what it is.
#
# $1       Name of file to check
# $inform  Command line argument to use with openssl for input file format
check_file() {
    # Check file is not in list of files to ignore
    for i in $ignore
    do
        case $1 in
        *$i)
            return
            ;;
        esac
    done

    # Use pattern matching to guess format
    case $1 in
    *key*) ;;
    *dh*) ;;
    *params*) ;;
    *priv*) ;;
    *pub*) ;;
    *dsa*) ;;
    *crl*)
        # Get the nextUpdate field from the CRL
        next_update=`openssl crl -in $file $inform -noout -nextupdate 2>&1`
        if [ "$?" != "0" ]
        then
            # Didn't work so report failure
            echo "$file not a crl"
        else
            # Get the date after the equal sign and check file
            next_update="${next_update#*=}"
            check_expiry $file "$next_update"
        fi
        ;;
    *)
        # Get the notAfter field from the certificate
        not_after=`openssl x509 -in $file $inform -noout -enddate 2>&1`
        if [ "$?" != "0" ]
        then
            # Didn't work, maybe wasn't a certificate, so report failure
            echo "$file not a certificate"
        else
            # Get the date after the equal sign and check file
            not_after="${not_after#*=}"
            check_expiry $file "$not_after"
        fi
        ;;
    esac
}

# Check all PEM files
inform="-inform PEM"
pem_files=`find . -name '*.pem'`
for file in $pem_files
do
    check_file $file
done

# Check all DER files
inform="-inform DER"
der_files=`find . -name '*.der'`
for file in $der_files
do
    check_file $file
done

# Return result of check
# 0 on success
# 1 on failure
return $expired
