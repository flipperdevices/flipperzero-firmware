-- tls_server.adb
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
with Ada.Text_IO.Bounded_IO;

with SPARK_Terminal; pragma Elaborate_All (SPARK_Terminal);

package body Tls_Server with SPARK_Mode is

   use type WolfSSL.Mode_Type;
   use type WolfSSL.Byte_Index;
   use type WolfSSL.Byte_Array;
   use type WolfSSL.Subprogram_Result;

   Success : WolfSSL.Subprogram_Result renames WolfSSL.Success;

   procedure Put (Char : Character) is
   begin
      Ada.Text_IO.Put (Char);
   end Put;

   procedure Put (Text : String) is
   begin
      Ada.Text_IO.Put (Text);
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

   Socket_Error : exception renames SPARK_Sockets.Socket_Error;

   Reuse_Address : Option_Name renames SPARK_Sockets.Reuse_Address;

   Socket_Level : Level_Type renames SPARK_Sockets.Socket_Level;

   Family_Inet : Family_Type renames SPARK_Sockets.Family_Inet;

   Any_Inet_Addr : Inet_Addr_Type renames SPARK_Sockets.Any_Inet_Addr;

   CERT_FILE : constant String := "../../../certs/server-cert.pem";
   KEY_FILE  : constant String := "../../../certs/server-key.pem";
   CA_FILE   : constant String := "../../../certs/client-cert.pem";

   subtype Byte_Array is WolfSSL.Byte_Array;

   Reply : constant Byte_Array := "I hear ya fa shizzle!";

   procedure Run (Ssl : in out WolfSSL.WolfSSL_Type;
                  Ctx : in out WolfSSL.Context_Type;
                  L   : in out SPARK_Sockets.Optional_Socket;
                  C   : in out SPARK_Sockets.Optional_Socket) is
      A : Sock_Addr_Type;
      P : constant Port_Type := 11111;

      Ch : Character;

      Result : WolfSSL.Subprogram_Result;
      Shall_Continue : Boolean := True;

      Input  : WolfSSL.Read_Result;
      Output : WolfSSL.Write_Result;
      Option : Option_Type;
   begin
      Result := WolfSSL.Initialize;
      if Result /= Success then
         Put_Line ("ERROR: Failed to initialize the WolfSSL library.");
         return;
      end if;

      SPARK_Sockets.Create_Socket (Socket => L);
      if not L.Exists then
         Put_Line ("ERROR: Failed to create socket.");
         return;
      end if;

      Option := (Name => Reuse_Address, Enabled => True);
      Result := SPARK_Sockets.Set_Socket_Option (Socket => L.Socket,
                                                 Level  => Socket_Level,
                                                 Option => Option);
      if Result /= Success then
         Put_Line ("ERROR: Failed to set socket option.");
         SPARK_Sockets.Close_Socket (L);
         return;
      end if;

      A := (Family => Family_Inet,
            Addr   => Any_Inet_Addr,
            Port   => P);
      Result := SPARK_Sockets.Bind_Socket (Socket  => L.Socket,
                                           Address => A);
      if Result /= Success then
         Put_Line ("ERROR: Failed to bind socket.");
         SPARK_Sockets.Close_Socket (L);
         return;
      end if;

      Result := SPARK_Sockets.Listen_Socket (Socket => L.Socket,
                                             Length => 5);
      if Result /= Success then
         Put_Line ("ERROR: Failed to configure listener socket.");
         SPARK_Sockets.Close_Socket (L);
         return;
      end if;

      --  Create and initialize WOLFSSL_CTX.
      WolfSSL.Create_Context (Method  => WolfSSL.TLSv1_3_Server_Method,
                              Context => Ctx);
      if not WolfSSL.Is_Valid (Ctx) then
         Put_Line ("ERROR: failed to create WOLFSSL_CTX.");
         SPARK_Sockets.Close_Socket (L);
         Set (Exit_Status_Failure);
         return;
      end if;

      --  Require mutual authentication.
      WolfSSL.Set_Verify
         (Context => Ctx,
          Mode    => WolfSSL.Verify_Peer & WolfSSL.Verify_Fail_If_No_Peer_Cert);

      --  Load server certificates into WOLFSSL_CTX.
      Result := WolfSSL.Use_Certificate_File (Context => Ctx,
                                              File    => CERT_FILE,
                                              Format  => WolfSSL.Format_Pem);
      if Result /= Success then
         Put ("ERROR: failed to load ");
         Put (CERT_FILE);
         Put (", please check the file.");
         New_Line;
         SPARK_Sockets.Close_Socket (L);
         WolfSSL.Free (Context => Ctx);
         Set (Exit_Status_Failure);
         return;
      end if;

      --  Load server key into WOLFSSL_CTX.
      Result := WolfSSL.Use_Private_Key_File (Context => Ctx,
                                              File    => KEY_FILE,
                                              Format  => WolfSSL.Format_Pem);
      if Result /= Success then
         Put ("ERROR: failed to load ");
         Put (KEY_FILE);
         Put (", please check the file.");
         New_Line;
         SPARK_Sockets.Close_Socket (L);
         WolfSSL.Free (Context => Ctx);
         Set (Exit_Status_Failure);
         return;
      end if;

      --  Load client certificate as "trusted" into WOLFSSL_CTX.
      Result := WolfSSL.Load_Verify_Locations (Context => Ctx,
                                               File    => CA_FILE,
                                               Path    => "");
      if Result /= Success then
         Put ("ERROR: failed to load ");
         Put (CA_FILE);
         Put (", please check the file.");
         New_Line;
         SPARK_Sockets.Close_Socket (L);
         WolfSSL.Free (Context => Ctx);
         Set (Exit_Status_Failure);
         return;
      end if;

      while Shall_Continue loop
         pragma Loop_Invariant (not C.Exists);
         pragma Loop_Invariant (not WolfSSL.Is_Valid (Ssl));
         pragma Loop_Invariant (WolfSSL.Is_Valid (Ctx));

         Put_Line ("Waiting for a connection...");
         SPARK_Sockets.Accept_Socket (Server  => L.Socket,
                                      Socket  => C,
                                      Address => A,
                                      Result  => Result);
         if Result /= Success then
            Put_Line ("ERROR: failed to accept the connection.");
            SPARK_Sockets.Close_Socket (L);
            WolfSSL.Free (Context => Ctx);
            return;
         end if;

         --  Create a WOLFSSL object.
         WolfSSL.Create_WolfSSL (Context => Ctx, Ssl => Ssl);
         if not WolfSSL.Is_Valid (Ssl) then
            Put_Line ("ERROR: failed to create WOLFSSL object.");
            SPARK_Sockets.Close_Socket (L);
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
            WolfSSL.Free (Ssl);
            SPARK_Sockets.Close_Socket (L);
            SPARK_Sockets.Close_Socket (C);
            WolfSSL.Free (Context => Ctx);
            Set (Exit_Status_Failure);
            return;
         end if;

         --  Establish TLS connection.
         Result := WolfSSL.Accept_Connection (Ssl);
         if Result /= Success then
            Put_Line ("Accept error.");
            WolfSSL.Free (Ssl);
            SPARK_Sockets.Close_Socket (L);
            SPARK_Sockets.Close_Socket (C);
            WolfSSL.Free (Context => Ctx);
            Set (Exit_Status_Failure);
            return;
         end if;

         Put_Line ("Client connected successfully.");

         Input := WolfSSL.Read (Ssl);
         if not  Input.Success then
            Put_Line ("Read error.");
            WolfSSL.Free (Ssl);
            SPARK_Sockets.Close_Socket (L);
            SPARK_Sockets.Close_Socket (C);
            WolfSSL.Free (Context => Ctx);
            Set (Exit_Status_Failure);
            return;
         end if;

         --  Print to stdout any data the client sends.
         for I in Input.Buffer'Range loop
            Ch := Character (Input.Buffer (I));
            if Ada.Characters.Handling.Is_Graphic (Ch) then
               Put (Ch);
            else
               null;
               --  Ignore the "newline" characters at end of message.
            end if;
         end loop;
         New_Line;

         --  Check for server shutdown command.
         if Input.Last >= 8  then
            if Input.Buffer (1 .. 8) = "shutdown" then
               Put_Line ("Shutdown command issued!");
               Shall_Continue := False;
            end if;
         end if;

         Output := WolfSSL.Write (Ssl, Reply);
         if not Output.Success then
            Put_Line ("ERROR: write failure.");
         elsif Output.Bytes_Written /= Reply'Length then
            Put_Line ("ERROR: failed to write full response.");
         end if;

         for I in 1 .. 3 loop
            Result := WolfSSL.Shutdown (Ssl);
            exit when Result = Success;
            delay 0.001;  --  Delay is expressed in seconds.
         end loop;
         if Result /= Success then
            Put_Line ("ERROR: Failed to shutdown WolfSSL context.");
         end if;
         WolfSSL.Free (Ssl);
         SPARK_Sockets.Close_Socket (C);

         Put_Line ("Shutdown complete.");
      end loop;
      SPARK_Sockets.Close_Socket (L);
      WolfSSL.Free (Context => Ctx);
      Result := WolfSSL.Finalize;
      if Result /= Success then
         Put_Line ("ERROR: Failed to finalize the WolfSSL library.");
         return;
      end if;
   end Run;

end Tls_Server;
