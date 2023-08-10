-- spark_sockets.ads
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

--  GNAT Library packages.
with GNAT.Sockets;

--  The WolfSSL package.
with WolfSSL;

--  This is a wrapper package around the GNAT.Sockets package.
--  GNAT.Sockets raises exceptions to signal errors but exceptions
--  are not supported by SPARK. This package converts raised exceptions
--  into returned enumeration values by functions indicating success
--  or failure.
--
--  The intended use of this package is to demonstrate the usage
--  of the WolfSSL Ada binding in Ada/SPARK code.
package SPARK_Sockets with SPARK_Mode is

   subtype Byte_Array is WolfSSL.Byte_Array;
   subtype Byte_Index is WolfSSL.Byte_Index; use type Byte_Index;

   subtype Port_Type is GNAT.Sockets.Port_Type;

   subtype Level_Type is GNAT.Sockets.Level_Type;

   subtype Socket_Type is GNAT.Sockets.Socket_Type;
   subtype Option_Name is GNAT.Sockets.Option_Name;
   subtype Option_Type is GNAT.Sockets.Option_Type;
   subtype Family_Type is GNAT.Sockets.Family_Type;

   subtype Sock_Addr_Type is GNAT.Sockets.Sock_Addr_Type;
   subtype Inet_Addr_Type is GNAT.Sockets.Inet_Addr_Type;

   Socket_Error : exception renames GNAT.Sockets.Socket_Error;

   Reuse_Address : Option_Name renames GNAT.Sockets.Reuse_Address;

   Socket_Level : Level_Type renames GNAT.Sockets.Socket_Level;

   Family_Inet : Family_Type renames GNAT.Sockets.Family_Inet;
   use type GNAT.Sockets.Family_Type;

   Any_Inet_Addr : Inet_Addr_Type renames GNAT.Sockets.Any_Inet_Addr;

   subtype Subprogram_Result is WolfSSL.Subprogram_Result;
   use type Subprogram_Result;

   Success : Subprogram_Result renames WolfSSL.Success;
   Failure : Subprogram_Result renames WolfSSL.Failure;

   type Optional_Inet_Addr (Exists : Boolean := False) is record
      case Exists is
         when True  => Addr : Inet_Addr_Type;
         when False => null;
      end case;
   end record;

   function Inet_Addr (Image : String) return Optional_Inet_Addr;

   type Optional_Socket (Exists : Boolean := False) is record
      case Exists is
         when True  => Socket : Socket_Type;
         when False => null;
      end case;
   end record;

   procedure Create_Socket (Socket : in out Optional_Socket) with
      Pre => not Socket.Exists;

   function Connect_Socket (Socket : Socket_Type;
                            Server : Sock_Addr_Type)
                            return Subprogram_Result;

   function To_C (Socket : Socket_Type) return Integer with Inline;

   --  Close a socket and more specifically a non-connected socket.
   procedure Close_Socket (Socket : in out Optional_Socket) with
      Pre  => Socket.Exists,
      Post => not Socket.Exists;

   function Set_Socket_Option (Socket : Socket_Type;
                               Level  : Level_Type;
                               Option : Option_Type)
                               return Subprogram_Result;
   --  Manipulate socket options.

   function Bind_Socket (Socket  : Socket_Type;
                         Address : Sock_Addr_Type)
                         return Subprogram_Result;

   function Listen_Socket (Socket : Socket_Type;
                           Length : Natural) return Subprogram_Result;
   --  To accept connections, a socket is first created with
   --  Create_Socket, a willingness to accept incoming connections and
   --  a queue Length for incoming connections are specified.
   --  The queue length of 15 is an example value that should be
   --  appropriate in usual cases. It can be adjusted according to each
   --  application's particular requirements.

   procedure Accept_Socket (Server  : Socket_Type;
                            Socket  : out Optional_Socket;
                            Address : out Sock_Addr_Type;
                            Result  : out Subprogram_Result) with
      Post => (if Result = Success then Socket.Exists else not Socket.Exists);

   procedure To_C (Item       : String;
                   Target     : out Byte_Array;
                   Count      : out Byte_Index) with
      Pre  => Item'Length <= Target'Length,
      Post => Count <= Target'Last;

   procedure To_Ada (Item     : Byte_Array;
                     Target   : out String;
                     Count    : out Natural) with
      Pre  => Item'Length <= Target'Length,
      Post => Count <= Target'Last;

end SPARK_Sockets;
