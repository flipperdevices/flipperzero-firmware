-- wolfssl.adb
--
-- Copyright (C) 2006-2023 wolfSSL Inc.
--
-- This file is part of wolfSSL.
--
-- wolfSSL is free software; you can redistribute it and/or modify
-- it under the terms of the GNU General Public License as published by
-- the Free Software Foundation; either version 2 of the License, or
-- (at your option) any later version.
--
-- wolfSSL is distributed in the hope that it will be useful,
-- but WITHOUT ANY WARRANTY; without even the implied warranty of
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
-- GNU General Public License for more details.
--
-- You should have received a copy of the GNU General Public License
-- along with this program; if not, write to the Free Software
-- Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1335, USA
--

with Interfaces.C.Strings;

package body WolfSSL is

   subtype size_t is Interfaces.C.size_t; use type size_t;

   subtype long is Interfaces.C.long;
   subtype unsigned_long is Interfaces.C.unsigned_long;

   WOLFSSL_SUCCESS : constant int := Get_WolfSSL_Success;

   function Initialize_WolfSSL return int with
     Convention    => C,
     External_Name => "wolfSSL_Init",
     Import        => True;

   function Finalize_WolfSSL return int with
     Convention    => C,
     External_Name => "wolfSSL_Cleanup",
     Import        => True;

   function Initialize return Subprogram_Result is
      Result : constant int := Initialize_WolfSSL;
   begin
      return Subprogram_Result (Result);
   end Initialize;

   function Finalize return Subprogram_Result is
      Result : constant int := Finalize_WolfSSL;
   begin
      return Subprogram_Result (Result);
   end Finalize;

   function Is_Valid (Context : Context_Type) return Boolean is
   begin
      return Context /= null;
   end Is_Valid;

   function WolfTLSv1_2_Server_Method return Method_Type with
     Convention    => C,
     External_Name => "wolfTLSv1_2_server_method",
     Import        => True;

   function TLSv1_2_Server_Method return Method_Type is
   begin
      return WolfTLSv1_2_Server_Method;
   end TLSv1_2_Server_Method;

   function WolfTLSv1_2_Client_Method return Method_Type with
     Convention    => C,
     External_Name => "wolfTLSv1_2_client_method",
     Import        => True;

   function TLSv1_2_Client_Method return Method_Type is
   begin
      return WolfTLSv1_2_Client_Method;
   end TLSv1_2_Client_Method;

   function WolfTLSv1_3_Server_Method return Method_Type with
     Convention    => C,
     External_Name => "wolfTLSv1_3_server_method",
     Import        => True;

   function TLSv1_3_Server_Method return Method_Type is
   begin
      return WolfTLSv1_3_Server_Method;
   end TLSv1_3_Server_Method;

   function WolfTLSv1_3_Client_Method return Method_Type with
     Convention    => C,
     External_Name => "wolfTLSv1_3_client_method",
     Import        => True;

   function TLSv1_3_Client_Method return Method_Type is
   begin
      return WolfTLSv1_3_Client_Method;
   end TLSv1_3_Client_Method;

   function WolfSSL_CTX_new (Method : Method_Type)
                             return Context_Type with
     Convention => C, External_Name => "wolfSSL_CTX_new", Import => True;

   procedure Create_Context (Method  : Method_Type;
                             Context : out Context_Type) is
   begin
      Context := WolfSSL_CTX_new (Method);
   end Create_Context;

   procedure WolfSSL_CTX_free (Context : Context_Type) with
     Convention => C, External_Name => "wolfSSL_CTX_free", Import => True;

   procedure Free (Context : in out Context_Type) is
   begin
      WolfSSL_CTX_free (Context);
      Context := null;
   end Free;

   type Opaque_X509_Store_Context is limited null record;
   type X509_Store_Context is access Opaque_X509_Store_Context with
     Convention => C;

   type Verify_Callback is access function
     (A : int;
      Context : X509_Store_Context)
      return int
     with Convention => C;

   procedure WolfSSL_CTX_Set_Verify (Context  : Context_Type;
                                     Mode     : int;
                                     Callback : Verify_Callback) with
     Convention    => C,
     External_Name => "wolfSSL_CTX_set_verify",
     Import        => True;
   --  This function sets the verification method for remote peers and
   --  also allows a verify callback to be registered with the SSL
   --  context. The verify callback will be called only when a
   --  verification failure has occurred. If no verify callback is
   --  desired, the NULL pointer can be used for verify_callback.
   --  The verification mode of peer certificates is a logically OR'd
   --  list of flags. The possible flag values include:
   --  SSL_VERIFY_NONE Client mode: the client will not verify the
   --  certificate received from the server and the handshake will
   --  continue as normal. Server mode: the server will not send a
   --  certificate request to the client. As such, client verification
   --  will not be enabled. SSL_VERIFY_PEER Client mode: the client will
   --  verify the certificate received from the server during the
   --  handshake. This is turned on by default in wolfSSL, therefore,
   --  using this option has no effect. Server mode: the server will send
   --  a certificate request to the client and verify the client
   --  certificate received. SSL_VERIFY_FAIL_IF_NO_PEER_CERT Client mode:
   --  no effect when used on the client side. Server mode:
   --  the verification will fail on the server side if the client fails
   --  to send a certificate when requested to do so (when using
   --  SSL_VERIFY_PEER on the SSL server).
   --  SSL_VERIFY_FAIL_EXCEPT_PSK Client mode: no effect when used on
   --  the client side. Server mode: the verification is the same as
   --  SSL_VERIFY_FAIL_IF_NO_PEER_CERT except in the case of a
   --  PSK connection. If a PSK connection is being made then the
   --  connection will go through without a peer cert.

   function "&" (Left, Right : Mode_Type) return Mode_Type is
      L : constant Unsigned_32 := Unsigned_32 (Left);
      R : constant Unsigned_32 := Unsigned_32 (Right);
   begin
      return Mode_Type (L and R);
   end "&";

   procedure Set_Verify (Context : Context_Type;
                         Mode    : Mode_Type) is
   begin
      WolfSSL_CTX_Set_Verify (Context  => Context,
                              Mode     => int (Mode),
                              Callback => null);
   end Set_Verify;

   function Use_Certificate_File (Context : Context_Type;
                                  File    : char_array;
                                  Format  : int)
                                  return int with
     Convention    => C,
     External_Name => "wolfSSL_CTX_use_certificate_file",
     Import        => True;

   function Use_Certificate_File (Context : Context_Type;
                                  File    : String;
                                  Format  : File_Format)
                                  return Subprogram_Result is
      Ctx : constant Context_Type := Context;
      C : size_t;
      F : char_array (1 .. File'Length + 1);
      Result : int;
   begin
      Interfaces.C.To_C (Item       => File,
                         Target     => F,
                         Count      => C,
                         Append_Nul => True);
      Result := Use_Certificate_File (Ctx, F (1 .. C), int (Format));
      return Subprogram_Result (Result);
   end Use_Certificate_File;

   function Use_Certificate_Buffer (Context : Context_Type;
                                    Input   : char_array;
                                    Size    : long;
                                    Format  : int)
                                    return int with
      Convention    => C,
      External_Name => "wolfSSL_CTX_use_certificate_buffer",
      Import        => True;

   function Use_Certificate_Buffer (Context : Context_Type;
                                    Input   : char_array;
                                    Format  : File_Format)
                                    return Subprogram_Result is
      Result : int;
   begin
      Result := Use_Certificate_Buffer (Context, Input,
                                        Input'Length, int (Format));
      return Subprogram_Result (Result);
   end Use_Certificate_Buffer;

   function Use_Private_Key_File (Context : Context_Type;
                                  File    : char_array;
                                  Format  : int)
                                  return int with
     Convention    => C,
     External_Name => "wolfSSL_CTX_use_PrivateKey_file",
     Import        => True;

   function Use_Private_Key_File (Context : Context_Type;
                                  File    : String;
                                  Format  : File_Format)
                                  return Subprogram_Result is
      Ctx : constant Context_Type := Context;
      C : size_t;
      F : char_array (1 .. File'Length + 1);
      Result : int;
   begin
      Interfaces.C.To_C (Item       => File,
                         Target     => F,
                         Count      => C,
                         Append_Nul => True);
      Result := Use_Private_Key_File (Ctx, F (1 .. C), int (Format));
      return Subprogram_Result (Result);
   end Use_Private_Key_File;

   function Use_Private_Key_Buffer (Context : Context_Type;
                                    Input   : char_array;
                                    Size    : long;
                                    Format  : int)
                                    return int with
     Convention    => C,
     External_Name => "wolfSSL_CTX_use_PrivateKey_buffer",
     Import        => True;

   function Use_Private_Key_Buffer (Context : Context_Type;
                                    Input   : Byte_Array;
                                    Format  : File_Format)
                                    return Subprogram_Result is
      Result : int;
   begin
      Result := Use_Private_Key_Buffer (Context, Input,
                                        Input'Length, int (Format));
      return Subprogram_Result (Result);
   end Use_Private_Key_Buffer;

   function Load_Verify_Locations1
      (Context : Context_Type;
       File    : char_array;
       Path    : char_array) return int with
      Convention    => C,
      External_Name => "wolfSSL_CTX_load_verify_locations",
      Import        => True;
   --  This function loads PEM-formatted CA certificate files into
   --  the SSL context (WOLFSSL_CTX). These certificates will be treated
   --  as trusted root certificates and used to verify certs received
   --  from peers during the SSL handshake. The root certificate file,
   --  provided by the file argument, may be a single certificate or a
   --  file containing multiple certificates. If multiple CA certs are
   --  included in the same file, wolfSSL will load them in the same order
   --  they are presented in the file. The path argument is a pointer to
   --  the name of a directory that contains certificates of trusted
   --  root CAs. If the value of file is not NULL, path may be specified
   --  as NULL if not needed. If path is specified and NO_WOLFSSL_DIR was
   --  not defined when building the library, wolfSSL will load all
   --  CA certificates located in the given directory. This function will
   --  attempt to load all files in the directory. This function expects
   --  PEM formatted CERT_TYPE file with header "--BEGIN CERTIFICATE--".

   subtype char_array_ptr is Interfaces.C.Strings.char_array_access;

   function Load_Verify_Locations2
      (Context : Context_Type;
       File    : char_array;
       Path    : char_array_ptr) return int with
      Convention    => C,
      External_Name => "wolfSSL_CTX_load_verify_locations",
      Import        => True;

   function Load_Verify_Locations3
      (Context : Context_Type;
       File    : char_array_ptr;
       Path    : char_array) return int with
      Convention    => C,
      External_Name => "wolfSSL_CTX_load_verify_locations",
      Import        => True;

   function Load_Verify_Locations4
      (Context : Context_Type;
       File    : char_array_ptr;
       Path    : char_array_ptr) return int with
      Convention    => C,
      External_Name => "wolfSSL_CTX_load_verify_locations",
      Import        => True;

   function Load_Verify_Locations (Context : Context_Type;
                                   File    : String;
                                   Path    : String)
                                   return Subprogram_Result is
      Ctx : constant Context_Type := Context;
      FC : size_t;  -- File Count, specifies the characters used in F.
      F : aliased char_array := (1 .. File'Length + 1 => '#');

      PC : size_t;  -- Path Count, specifies the characters used in P.
      P : aliased char_array := (1 .. Path'Length + 1 => '#');

      Result : int;
   begin
      if File = "" then
         if Path = "" then
            Result := Load_Verify_Locations4 (Ctx, null, null);
         else
            Interfaces.C.To_C (Item       => Path,
                               Target     => P,
                               Count      => PC,
                               Append_Nul => True);
            Result := Load_Verify_Locations3 (Ctx, null, P);
         end if;
      else
         Interfaces.C.To_C (Item       => File,
                            Target     => F,
                            Count      => FC,
                            Append_Nul => True);
         if Path = "" then
            Result := Load_Verify_Locations2 (Ctx, F, null);
         else
            Interfaces.C.To_C (Item       => Path,
                               Target     => P,
                               Count      => PC,
                               Append_Nul => True);
            Interfaces.C.To_C (Item       => Path,
                               Target     => P,
                               Count      => PC,
                               Append_Nul => True);
            Result := Load_Verify_Locations1 (Context => Ctx,
                                              File    => F,
                                              Path    => P);
         end if;
      end if;
      return Subprogram_Result (Result);
   end Load_Verify_Locations;

   function Load_Verify_Buffer
      (Context : Context_Type;
       Input   : char_array;
       Size    : int;
       Format  : int) return int with
      Convention    => C,
      External_Name => "wolfSSL_CTX_load_verify_buffer",
      Import        => True;

   function Load_Verify_Buffer (Context : Context_Type;
                                Input   : Byte_Array;
                                Format  : File_Format)
                                return Subprogram_Result is
      Result : int;
   begin
      Result := Load_Verify_Buffer (Context => Context,
                                     Input   => Input,
                                     Size    => Input'Length,
                                     Format  => int(Format));
      return Subprogram_Result (Result);
   end Load_Verify_Buffer;

   function Is_Valid (Ssl : WolfSSL_Type) return Boolean is
   begin
      return Ssl /= null;
   end Is_Valid;

   function WolfSSL_New (Context : Context_Type)
                         return WolfSSL_Type with
     Convention    => C,
     External_Name => "wolfSSL_new",
     Import        => True;

   procedure Create_WolfSSL (Context : Context_Type;
                             Ssl     : out WolfSSL_Type) is
   begin
      Ssl := WolfSSL_New (Context);
   end Create_WolfSSL;

   function Use_Certificate_File (Ssl     : WolfSSL_Type;
                                  File    : char_array;
                                  Format  : int)
                                  return int with
     Convention    => C,
     External_Name => "wolfSSL_use_certificate_file",
     Import        => True;

   function Use_Certificate_File (Ssl     : WolfSSL_Type;
                                  File    : String;
                                  Format  : File_Format)
                                  return Subprogram_Result is
      C : size_t;
      F : char_array (1 .. File'Length + 1);
      Result : int;
   begin
      Interfaces.C.To_C (Item       => File,
                         Target     => F,
                         Count      => C,
                         Append_Nul => True);
      Result := Use_Certificate_File (Ssl, F (1 .. C), int (Format));
      return Subprogram_Result (Result);
   end Use_Certificate_File;

   function Use_Certificate_Buffer (Ssl     : WolfSSL_Type;
                                    Input   : char_array;
                                    Size    : long;
                                    Format  : int)
                                    return int with
      Convention    => C,
      External_Name => "wolfSSL_use_certificate_buffer",
      Import        => True;

   function Use_Certificate_Buffer (Ssl     : WolfSSL_Type;
                                    Input   : char_array;
                                    Format  : File_Format)
                                    return Subprogram_Result is
      Result : int;
   begin
      Result := Use_Certificate_Buffer (Ssl, Input,
                                        Input'Length, int (Format));
      return Subprogram_Result (Result);
   end Use_Certificate_Buffer;

   function Use_Private_Key_File (Ssl     : WolfSSL_Type;
                                  File    : char_array;
                                  Format  : int)
                                   return int with
      Convention    => C,
      External_Name => "wolfSSL_use_PrivateKey_file",
      Import        => True;

   function Use_Private_Key_File (Ssl     : WolfSSL_Type;
                                  File    : String;
                                  Format  : File_Format)
                                  return Subprogram_Result is
      C : size_t;
      F : char_array (1 .. File'Length + 1);
      Result : int;
   begin
      Interfaces.C.To_C (Item       => File,
                         Target     => F,
                         Count      => C,
                         Append_Nul => True);
      Result := Use_Private_Key_File (Ssl, F (1 .. C), int (Format));
      return Subprogram_Result (Result);
   end Use_Private_Key_File;

   function Use_Private_Key_Buffer (Ssl     : WolfSSL_Type;
                                    Input   : char_array;
                                    Size    : long;
                                    Format  : int)
                                    return int with
     Convention    => C,
     External_Name => "wolfSSL_use_PrivateKey_buffer",
     Import        => True;

   function Use_Private_Key_Buffer (Ssl     : WolfSSL_Type;
                                    Input   : Byte_Array;
                                    Format  : File_Format)
                                    return Subprogram_Result is
      Result : int;
   begin
      Result := Use_Private_Key_Buffer (Ssl, Input,
                                        Input'Length, int (Format));
      return Subprogram_Result (Result);
   end Use_Private_Key_Buffer;

   function WolfSSL_Set_Fd (Ssl : WolfSSL_Type; Fd : int) return int with
     Convention    => C,
     External_Name => "wolfSSL_set_fd",
     Import        => True;

   function Attach (Ssl    : WolfSSL_Type;
                    Socket : Integer)
                    return Subprogram_Result is
      Result : int := WolfSSL_Set_Fd (Ssl, int (Socket));
   begin
      return Subprogram_Result (Result);
   end Attach;

   procedure WolfSSL_Keep_Arrays (Ssl : WolfSSL_Type) with
      Convention    => C,
      External_Name => "wolfSSL_KeepArrays",
      Import        => True;

   procedure Keep_Arrays (Ssl : WolfSSL_Type) is
   begin
      WolfSSL_Keep_Arrays (Ssl);
   end Keep_Arrays;

   function WolfSSL_Accept (Ssl : WolfSSL_Type) return int with
     Convention    => C,
     External_Name => "wolfSSL_accept",
     Import        => True;

   function Accept_Connection (Ssl : WolfSSL_Type)
                               return Subprogram_Result is
      Result : int := WolfSSL_Accept (Ssl);
   begin
      return Subprogram_Result (Result);
   end Accept_Connection;

   procedure WolfSSL_Free_Arrays (Ssl : WolfSSL_Type) with
     Convention    => C,
     External_Name => "wolfSSL_FreeArrays",
     Import        => True;

   procedure Free_Arrays (Ssl : WolfSSL_Type) is
   begin
      WolfSSL_Free_Arrays (Ssl);
   end Free_Arrays;

   function WolfSSL_Read (Ssl  : WolfSSL_Type;
                          Data : out char_array;
                          Sz   : int) return int with
     Convention    => C,
     External_Name => "wolfSSL_read",
     Import        => True;
   --  This function reads sz bytes from the SSL session (ssl) internal
   --  read buffer into the buffer data. The bytes read are removed from
   --  the internal receive buffer. If necessary wolfSSL_read() will
   --  negotiate an SSL/TLS session if the handshake has not already
   --  been performed yet by wolfSSL_connect() or wolfSSL_accept().
   --  The SSL/TLS protocol uses SSL records which have a maximum size
   --  of 16kB (the max record size can be controlled by the
   --  MAX_RECORD_SIZE define in /wolfssl/internal.h). As such, wolfSSL
   --  needs to read an entire SSL record internally before it is able
   --  to process and decrypt the record. Because of this, a call to
   --  wolfSSL_read() will only be able to return the maximum buffer
   --  size which has been decrypted at the time of calling. There may
   --  be additional not-yet-decrypted data waiting in the internal
   --  wolfSSL receive buffer which will be retrieved and decrypted with
   --  the next call to wolfSSL_read(). If sz is larger than the number
   --  of bytes in the internal read buffer, SSL_read() will return
   --  the bytes available in the internal read buffer. If no bytes are
   --  buffered in the internal read buffer yet, a call to wolfSSL_read()
   --  will trigger processing of the next record.
   --
   --  The integer returned is the number of bytes read upon success.
   --  0 will be returned upon failure. This may be caused by a either
   --  a clean (close notify alert) shutdown or just that the peer closed
   --  the connection. Call wolfSSL_get_error() for the specific
   --  error code. SSL_FATAL_ERROR will be returned upon failure when
   --  either an error occurred or, when using non-blocking sockets,
   --  the SSL_ERROR_WANT_READ or SSL_ERROR_WANT_WRITE error was received
   --  and and the application needs to call wolfSSL_read() again.
   --  Use wolfSSL_get_error() to get a specific error code.

   function Read (Ssl : WolfSSL_Type) return Read_Result is
      Data   : char_array (1 .. Byte_Index'Last);
      Size   : int;
   begin
      Size := WolfSSL_Read (Ssl, Data, int (Byte_Index'Last));
      if Size <= 0 then
         return (Success => False,
                 Last    => 0,
                 Code    => Subprogram_Result (Size));
      else
         return (Success => True,
                 Last    => Byte_Index (Size),
                 Buffer  => Data (1 .. Byte_Index (Size)));
      end if;
   end Read;

   function WolfSSL_Write (Ssl  : WolfSSL_Type;
                           Data : char_array;
                           Sz   : int) return int with
     Convention    => C,
     External_Name => "wolfSSL_write",
     Import        => True;

   function Write (Ssl  : WolfSSL_Type;
                   Data : Byte_Array) return Write_Result is
      Size   : constant int := Data'Length;
      Result : int;
   begin
      Result := WolfSSL_Write (Ssl, Data, Size);
      if Result > 0 then
         return (Success       => True,
                 Bytes_Written => Byte_Index (Result));
      else
         return (Success => False, Code => Subprogram_Result (Result));
      end if;
   end Write;

   function WolfSSL_Shutdown (Ssl : WolfSSL_Type) return int with
     Convention    => C,
     External_Name => "wolfSSL_shutdown",
     Import        => True;

   function Shutdown (Ssl : WolfSSL_Type) return Subprogram_Result is
      Result : constant int := WolfSSL_Shutdown (Ssl);
   begin
      return Subprogram_Result (Result);
   end Shutdown;

   function WolfSSL_Connect (Ssl : WolfSSL_Type) return int with
      Convention    => C,
      External_Name => "wolfSSL_connect",
      Import        => True;

   function Connect (Ssl : WolfSSL_Type) return Subprogram_Result is
      Result : constant int := WolfSSL_Connect (Ssl);
   begin
      return Subprogram_Result (Result);
   end Connect;

   procedure WolfSSL_Free (Ssl : WolfSSL_Type) with
      Convention    => C,
      External_Name => "wolfSSL_free",
      Import        => True;

   procedure Free (Ssl : in out WolfSSL_Type) is
   begin
      if Ssl /= null then
         WolfSSL_Free (Ssl);
      end if;
      Ssl := null;
   end Free;

   function WolfSSL_Get_Error (Ssl : WolfSSL_Type;
                               Ret : int) return int with
      Convention    => C,
      External_Name => "wolfSSL_get_error",
      Import        => True;

   function Get_Error (Ssl    : WolfSSL_Type;
                       Result : Subprogram_Result) return Error_Code is
   begin
      return Error_Code (WolfSSL_Get_Error (Ssl, int (Result)));
   end Get_Error;

   procedure WolfSSL_Error_String (Error : unsigned_long;
                                   Data  : out Byte_Array;
                                   Size  : unsigned_long) with
      Convention    => C,
      External_Name => "wolfSSL_ERR_error_string_n",
      Import        => True;

   function Error (Code : Error_Code) return Error_Message is
      S : String (1 .. Error_Message_Index'Last);
      B : Byte_Array (1 .. size_t (Error_Message_Index'Last));
      C : Natural;
   begin
      WolfSSL_Error_String (Error => unsigned_long (Code),
                            Data  => B,
                            Size  => unsigned_long (B'Last));
      Interfaces.C.To_Ada (Item     => B,
                           Target   => S,
                           Count    => C,
                           Trim_Nul => True);
      return (Last => C,
              Text => S (1 .. C));
   end Error;

   function Get_WolfSSL_Max_Error_Size return int with
     Convention    => C,
     External_Name => "get_wolfssl_max_error_size",
     Import        => True;

   function Max_Error_Size return Natural is
   begin
      return Natural (Get_WolfSSL_Max_Error_Size);
   end Max_Error_Size;

end WolfSSL;
