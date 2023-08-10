-- tls_client.adb
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

--  Ada Standard Library packages.
with Ada.Characters.Handling;
with Ada.Strings.Bounded;
with Ada.Text_IO;
with Interfaces.C;

with SPARK_Terminal;

package body Tls_Client with SPARK_Mode is

   use type WolfSSL.Mode_Type;
   use type WolfSSL.Byte_Index;
   use type WolfSSL.Byte_Array;
   use type WolfSSL.Subprogram_Result;

   subtype Byte_Index is WolfSSL.Byte_Index;

   Success : WolfSSL.Subprogram_Result renames WolfSSL.Success;

   subtype Byte_Type is WolfSSL.Byte_Type;

   package Natural_IO is new Ada.Text_IO.Integer_IO (Natural);

   procedure Put (Text : String) is
   begin
      Ada.Text_IO.Put (Text);
   end Put;

   procedure Put (Number : Natural) is
   begin
      Natural_IO.Put (Item => Number, Width => 0, Base => 10);
   end Put;

   procedure Put (Number : Byte_Index) is
   begin
      Natural_IO.Put (Item => Natural (Number), Width => 0, Base => 10);
   end Put;

   procedure Put_Line (Text : String) is
   begin
      Ada.Text_IO.Put_Line (Text);
   end Put_Line;

   procedure New_Line is
   begin
      Ada.Text_IO.New_Line;
   end New_Line;

   subtype Exit_Status is SPARK_Terminal.Exit_Status;

   Exit_Status_Success : Exit_Status renames SPARK_Terminal.Exit_Status_Success;
   Exit_Status_Failure : Exit_Status renames SPARK_Terminal.Exit_Status_Failure;

   procedure Set (Status : Exit_Status) with Global => null is
   begin
      SPARK_Terminal.Set_Exit_Status (Status);
   end Set;

   subtype Port_Type is SPARK_Sockets.Port_Type;

   subtype Level_Type is SPARK_Sockets.Level_Type;

   subtype Socket_Type is SPARK_Sockets.Socket_Type;
   subtype Option_Name is SPARK_Sockets.Option_Name;
   subtype Option_Type is SPARK_Sockets.Option_Type;
   subtype Family_Type is SPARK_Sockets.Family_Type;

   subtype Sock_Addr_Type is SPARK_Sockets.Sock_Addr_Type;
   subtype Inet_Addr_Type is SPARK_Sockets.Inet_Addr_Type;

   use type Family_Type;

   Socket_Error : exception renames SPARK_Sockets.Socket_Error;

   Reuse_Address : Option_Name renames SPARK_Sockets.Reuse_Address;

   Socket_Level : Level_Type renames SPARK_Sockets.Socket_Level;

   Family_Inet : Family_Type renames SPARK_Sockets.Family_Inet;

   Any_Inet_Addr : Inet_Addr_Type renames SPARK_Sockets.Any_Inet_Addr;

   CERT_FILE : constant String := "../../../certs/client-cert.pem";
   KEY_FILE  : constant String := "../../../certs/client-key.pem";
   CA_FILE   : constant String := "../../../certs/ca-cert.pem";

   subtype Byte_Array is WolfSSL.Byte_Array;

   function Argument_Count return Natural renames
      SPARK_Terminal.Argument_Count;

   function Argument (Number : Positive) return String with
      Pre => Number <= Argument_Count;

   function Argument (Number : Positive) return String is
   begin
      return SPARK_Terminal.Argument (Number);
   end Argument;

   procedure Run (Ssl    : in out WolfSSL.WolfSSL_Type;
                  Ctx    : in out WolfSSL.Context_Type;
                  Client : in out SPARK_Sockets.Optional_Socket) is
      A : Sock_Addr_Type;
      C : SPARK_Sockets.Optional_Socket renames Client;
      D : Byte_Array (1 .. 200);
      P : constant Port_Type := 11111;

      Addr : SPARK_Sockets.Optional_Inet_Addr;

      Count : WolfSSL.Byte_Index;

      Text : String (1 .. 200);
      Last : Natural;

      Input  : WolfSSL.Read_Result;
      Output : WolfSSL.Write_Result;

      Result : WolfSSL.Subprogram_Result;
   begin
      Result := WolfSSL.Initialize;
      if Result /= Success then
         Put_Line ("ERROR: Failed to initialize the WolfSSL library.");
         return;
      end if;

      if Argument_Count < 1 then
         Put_Line ("usage: tcl_client <IPv4 address>");
         return;
      end if;
      SPARK_Sockets.Create_Socket (C);
      if not C.Exists then
         Put_Line ("ERROR: Failed to create socket.");
         return;
      end if;

      Addr := SPARK_Sockets.Inet_Addr (Argument (1));
      if not Addr.Exists or
         (Addr.Exists and then Addr.Addr.Family /= Family_Inet)
      then
         Put_Line ("ERROR: please specify IPv4 address.");
         SPARK_Sockets.Close_Socket (C);
         Set (Exit_Status_Failure);
         return;
      end if;
      A := (Family => Family_Inet,
            Addr   => Addr.Addr,
            Port   => P);

      Result := SPARK_Sockets.Connect_Socket (Socket => C.Socket,
                                              Server => A);
      if Result /= Success then
         Put_Line ("ERROR: Failed to connect to server.");
         SPARK_Sockets.Close_Socket (C);
         Set (Exit_Status_Failure);
         return;
      end if;

      --  Create and initialize WOLFSSL_CTX.
      WolfSSL.Create_Context (Method  => WolfSSL.TLSv1_3_Client_Method,
                              Context => Ctx);
      if not WolfSSL.Is_Valid (Ctx) then
         Put_Line ("ERROR: failed to create WOLFSSL_CTX.");
         SPARK_Sockets.Close_Socket (C);
         Set (Exit_Status_Failure);
         return;
      end if;

      --  Load client certificate into WOLFSSL_CTX.
      Result := WolfSSL.Use_Certificate_File (Context => Ctx,
                                              File    => CERT_FILE,
                                              Format  => WolfSSL.Format_Pem);
      if Result /= Success then
         Put ("ERROR: failed to load ");
         Put (CERT_FILE);
         Put (", please check the file.");
         New_Line;
         SPARK_Sockets.Close_Socket (C);
         WolfSSL.Free (Context => Ctx);
         Set (Exit_Status_Failure);
         return;
      end if;

      --  Load client key into WOLFSSL_CTX.
      Result := WolfSSL.Use_Private_Key_File (Context => Ctx,
                                              File    => KEY_FILE,
                                              Format  => WolfSSL.Format_Pem);
      if Result /= Success then
         Put ("ERROR: failed to load ");
         Put (KEY_FILE);
         Put (", please check the file.");
         New_Line;
         SPARK_Sockets.Close_Socket (C);
         WolfSSL.Free (Context => Ctx);
         Set (Exit_Status_Failure);
         return;
      end if;

      --  Load CA certificate into WOLFSSL_CTX.
      Result := WolfSSL.Load_Verify_Locations (Context => Ctx,
                                               File    => CA_FILE,
                                               Path    => "");
      if Result /= Success then
         Put ("ERROR: failed to load ");
         Put (CA_FILE);
         Put (", please check the file.");
         New_Line;
         SPARK_Sockets.Close_Socket (C);
         WolfSSL.Free (Context => Ctx);
         Set (Exit_Status_Failure);
         return;
      end if;

      --  Create a WOLFSSL object.
      WolfSSL.Create_WolfSSL (Context => Ctx, Ssl => Ssl);
      if not WolfSSL.Is_Valid (Ssl) then
         Put_Line ("ERROR: failed to create WOLFSSL object.");
         SPARK_Sockets.Close_Socket (C);
         WolfSSL.Free (Context => Ctx);
         Set (Exit_Status_Failure);
         return;
      end if;

      --  Attach wolfSSL to the socket.
      Result := WolfSSL.Attach (Ssl    => Ssl,
                                Socket => SPARK_Sockets.To_C (C.Socket));
      if Result /= Success then
         Put_Line ("ERROR: Failed to set the file descriptor.");
         SPARK_Sockets.Close_Socket (C);
         WolfSSL.Free (Ssl);
         WolfSSL.Free (Context => Ctx);
         Set (Exit_Status_Failure);
         return;
      end if;

      Result := WolfSSL.Connect (Ssl);
      if Result /= Success then
         Put_Line ("ERROR: failed to connect to wolfSSL.");
         SPARK_Sockets.Close_Socket (C);
         WolfSSL.Free (Ssl);
         WolfSSL.Free (Context => Ctx);
         Set (Exit_Status_Failure);
         return;
      end if;

      Put ("Message for server: ");
      Ada.Text_IO.Get_Line (Text, Last);

      SPARK_Sockets.To_C (Item       => Text (1 .. Last),
                          Target     => D,
                          Count      => Count);
      Output := WolfSSL.Write (Ssl  => Ssl,
                               Data => D (1 .. Count));
      if not Output.Success then
         Put ("ERROR: write failure");
         New_Line;
         SPARK_Sockets.Close_Socket (C);
         WolfSSL.Free (Ssl);
         WolfSSL.Free (Context => Ctx);
         return;
      end if;

      if Natural (Output.Bytes_Written) < Last then
         Put ("ERROR: failed to write entire message");
         New_Line;
         Put (Output.Bytes_Written);
         Put (" bytes of ");
         Put (Last);
         Put ("bytes were sent");
         New_Line;
         SPARK_Sockets.Close_Socket (C);
         WolfSSL.Free (Ssl);
         WolfSSL.Free (Context => Ctx);
         return;
      end if;

      Input := WolfSSL.Read (Ssl);
      if not Input.Success then
         Put_Line ("Read error.");
         Set (Exit_Status_Failure);
         SPARK_Sockets.Close_Socket (C);
         WolfSSL.Free (Ssl);
         WolfSSL.Free (Context => Ctx);
         return;
      end if;
      if Input.Buffer'Length > Text'Length then
         SPARK_Sockets.To_Ada (Item     => Input.Buffer (1 .. 200),
                               Target   => Text,
                               Count    => Last);
      else
         SPARK_Sockets.To_Ada (Item     => Input.Buffer,
                               Target   => Text,
                               Count    => Last);
      end if;
      Put ("Server: ");
      Put (Text (1 .. Last));
      New_Line;

      SPARK_Sockets.Close_Socket (C);
      WolfSSL.Free (Ssl);
      WolfSSL.Free (Context => Ctx);
      Result := WolfSSL.Finalize;
      if Result /= Success then
         Put_Line ("ERROR: Failed to finalize the WolfSSL library.");
      end if;
   end Run;

end Tls_Client;
