-- tls_client_main.adb
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

with Tls_Client;    pragma Elaborate_All (Tls_Client);
with SPARK_Sockets; pragma Elaborate_All (SPARK_Sockets);
with WolfSSL;       pragma Elaborate_All (WolfSSL);
--  Application entry point for the Ada translation of the
--  tls client v1.3 example in C.
procedure Tls_Client_Main is
   Ssl : WolfSSL.WolfSSL_Type;
   Ctx : WolfSSL.Context_Type;
   C   : SPARK_Sockets.Optional_Socket;
begin
   Tls_Client.Run (Ssl, Ctx, Client => C);
end Tls_Client_Main;
