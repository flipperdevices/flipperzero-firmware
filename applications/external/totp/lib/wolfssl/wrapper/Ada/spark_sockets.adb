-- spark_sockets.adb
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

with Interfaces.C;

package body SPARK_Sockets is

   function Inet_Addr (Image : String) return Optional_Inet_Addr is
      A : Inet_Addr_Type;
   begin
      A := GNAT.Sockets.Inet_Addr (Image);
      return (Exists => True, Addr => A);
   exception
      when others =>
         return (Exists => False);
   end Inet_Addr;

   procedure Create_Socket (Socket : in out Optional_Socket) is
      S : Socket_Type;
   begin
      GNAT.Sockets.Create_Socket (S);
      Socket := (Exists => True, Socket => S);
   exception
      when others =>
         Socket := (Exists => False);
   end Create_Socket;

   function Connect_Socket (Socket : Socket_Type;
                            Server : Sock_Addr_Type)
                            return Subprogram_Result is
   begin
      GNAT.Sockets.Connect_Socket (Socket, Server);
      return Success;
   exception
      when others =>
         return Failure;
   end Connect_Socket;

   function To_C (Socket : Socket_Type) return Integer is
   begin
      --  The call to GNAT.Sockets.To_C can never raise an exception.
      return GNAT.Sockets.To_C (Socket);
   end To_C;

   procedure Close_Socket (Socket : in out Optional_Socket) is
   begin
      GNAT.Sockets.Close_Socket (Socket.Socket);
      Socket := (Exists => False);
   end Close_Socket;

   function Set_Socket_Option (Socket : Socket_Type;
                               Level  : Level_Type;
                               Option : Option_Type)
                               return Subprogram_Result is
   begin
      GNAT.Sockets.Set_Socket_Option (Socket, Level, Option);
      return Success;
   exception
      when others =>
         return Failure;
   end Set_Socket_Option;

   function Bind_Socket (Socket  : Socket_Type;
                         Address : Sock_Addr_Type)
                         return Subprogram_Result is
   begin
      GNAT.Sockets.Bind_Socket (Socket, Address);
      return Success;
   exception
      when others =>
         return Failure;
   end Bind_Socket;

   function Listen_Socket (Socket : Socket_Type;
                           Length : Natural) return Subprogram_Result is
   begin
      GNAT.Sockets.Listen_Socket (Socket, Length);
      return Success;
   exception
      when others =>
         return Failure;
   end Listen_Socket;

   procedure Accept_Socket (Server  : Socket_Type;
                            Socket  : out Optional_Socket;
                            Address : out Sock_Addr_Type;
                            Result  : out Subprogram_Result) is
      C : Socket_Type;
   begin
      GNAT.Sockets.Accept_Socket (Server, C, Address);
      Socket := (Exists => True, Socket => C);
      Result := Success;
   exception
      when others =>
         Socket := (Exists => False);
         Address := (Family => GNAT.Sockets.Family_Unspec);
         Result := Failure;
   end Accept_Socket;

   procedure To_C (Item       : String;
                   Target     : out Byte_Array;
                   Count      : out Byte_Index) is
   begin
      Interfaces.C.To_C (Item       => Item,
                         Target     => Target,
                         Count      => Count,
                         Append_Nul => False);
   end To_C;

   procedure To_Ada (Item     : Byte_Array;
                     Target   : out String;
                     Count    : out Natural) is
   begin
      Interfaces.C.To_Ada (Item     => Item,
                           Target   => Target,
                           Count    => Count,
                           Trim_Nul => False);
   end To_Ada;

end SPARK_Sockets;
