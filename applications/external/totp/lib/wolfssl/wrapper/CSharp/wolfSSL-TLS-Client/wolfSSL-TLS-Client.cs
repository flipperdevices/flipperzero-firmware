/* wolfSSL-TLS-Client.cs
 *
 * Copyright (C) 2006-2023 wolfSSL Inc.
 *
 * This file is part of wolfSSL.
 *
 * wolfSSL is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * wolfSSL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1335, USA
 */


using System;
using System.Runtime.InteropServices;
using System.Text;
using System.IO;
using System.Net;
using System.Net.Sockets;
using wolfSSL.CSharp;

public class wolfSSL_TLS_Client
{
    /// <summary>
    /// Example of a logging function
    /// </summary>
    /// <param name="lvl">level of log</param>
    /// <param name="msg">message to log</param>
    public static void standard_log(int lvl, StringBuilder msg)
    {
        Console.WriteLine(msg);
    }


    private static void clean(IntPtr ssl, IntPtr ctx)
    {
        wolfssl.free(ssl);
        wolfssl.CTX_free(ctx);
        wolfssl.Cleanup();
    }

    /// <summary>
    /// Verification callback
    /// </summary>
    /// <param name="preverify">1=Verify Okay, 0=Failure</param>
    /// <param name="x509_ctx">Certificate in WOLFSSL_X509_STORE_CTX format</param>
    private static int myVerify(int preverify, IntPtr x509_ctx)
    {
        /* Use the provided verification */
        /* Can optionally override failures by returning non-zero value */
        return preverify;
    }

    public static void Main(string[] args)
    {
        IntPtr ctx;
        IntPtr ssl;
        Socket tcp;

        /* These paths should be changed for use */
        string caCert = @"ca-cert.pem";
        StringBuilder dhparam = new StringBuilder("dh2048.pem");

        StringBuilder buff = new StringBuilder(1024);
        StringBuilder reply = new StringBuilder("Hello, this is the wolfSSL C# wrapper");

        //example of function used for setting logging
        wolfssl.SetLogging(standard_log);

        wolfssl.Init();


        Console.WriteLine("Calling ctx Init from wolfSSL");
        ctx = wolfssl.CTX_new(wolfssl.usev23_client());
        if (ctx == IntPtr.Zero)
        {
            Console.WriteLine("Error in creating ctx structure");
            return;
        }
        Console.WriteLine("Finished init of ctx .... now load in CA");


        if (!File.Exists(caCert))
        {
            Console.WriteLine("Could not find CA cert file");
            wolfssl.CTX_free(ctx);
            return;
        }


        if (wolfssl.CTX_load_verify_locations(ctx, caCert, null)
            != wolfssl.SUCCESS)
        {
            Console.WriteLine("Error loading CA cert");
        }

        StringBuilder ciphers = new StringBuilder(new String(' ', 4096));
        wolfssl.get_ciphers(ciphers, 4096);
        Console.WriteLine("Ciphers : " + ciphers.ToString());

        /* Uncomment Section to enable specific cipher suite */
#if false
        ciphers = new StringBuilder("ECDHE-ECDSA-AES128-GCM-SHA256");
        if (wolfssl.CTX_set_cipher_list(ctx, ciphers) != wolfssl.SUCCESS)
        {
            Console.WriteLine("ERROR CTX_set_cipher_list()");
            wolfssl.CTX_free(ctx);
            return;
        }
#endif

        short minDhKey = 128;
        wolfssl.CTX_SetMinDhKey_Sz(ctx, minDhKey);

        /* Setup Verify Callback */
        if (wolfssl.CTX_set_verify(ctx, wolfssl.SSL_VERIFY_PEER, myVerify)
            != wolfssl.SUCCESS)
        {
            Console.WriteLine("Error setting verify callback!");
        }


        /* set up TCP socket */
        tcp = new Socket(AddressFamily.InterNetwork, SocketType.Stream,
                              ProtocolType.Tcp);
        try
        {
            tcp.Connect("localhost", 11111);
        }
        catch (Exception e)
        {
            Console.WriteLine("tcp.Connect() error " + e.ToString());
            wolfssl.CTX_free(ctx);
            return;
        }
        if (!tcp.Connected)
        {
            Console.WriteLine("tcp.Connect() failed!");
            tcp.Close();
            wolfssl.CTX_free(ctx);
            return;
        }

        Console.WriteLine("Connected TCP");
        ssl = wolfssl.new_ssl(ctx);
        if (ssl == IntPtr.Zero)
        {
            Console.WriteLine("Error in creating ssl object");
            wolfssl.CTX_free(ctx);
            return;
        }

        Console.WriteLine("Connection made wolfSSL_connect ");
        if (wolfssl.set_fd(ssl, tcp) != wolfssl.SUCCESS)
        {
            /* get and print out the error */
            Console.WriteLine(wolfssl.get_error(ssl));
            tcp.Close();
            clean(ssl, ctx);
            return;
        }

        wolfssl.SetTmpDH_file(ssl, dhparam, wolfssl.SSL_FILETYPE_PEM);

        if (wolfssl.connect(ssl) != wolfssl.SUCCESS)
        {
            /* get and print out the error */
            Console.WriteLine(wolfssl.get_error(ssl));
            tcp.Close();
            clean(ssl, ctx);
            return;
        }

        /* print out results of TLS/SSL accept */
        Console.WriteLine("SSL version is " + wolfssl.get_version(ssl));
        Console.WriteLine("SSL cipher suite is " + wolfssl.get_current_cipher(ssl));


        if (wolfssl.write(ssl, reply, reply.Length) != reply.Length)
        {
            Console.WriteLine("Error in write");
            tcp.Close();
            clean(ssl, ctx);
            return;
        }

        /* read and print out the message then reply */
        if (wolfssl.read(ssl, buff, 1023) < 0)
        {
            Console.WriteLine("Error in read");
            tcp.Close();
            clean(ssl, ctx);
            return;
        }
        Console.WriteLine(buff);

        wolfssl.shutdown(ssl);
        tcp.Close();
        clean(ssl, ctx);
    }
}
