-- tls_client.ads
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

--  The WolfSSL package.
with WolfSSL; pragma Elaborate_All (WolfSSL);

with SPARK_Sockets; pragma Elaborate_All (SPARK_Sockets);

package Tls_Client with SPARK_Mode is

   procedure Run (Ssl    : in out WolfSSL.WolfSSL_Type;
                  Ctx    : in out WolfSSL.Context_Type;
                  Client : in out SPARK_Sockets.Optional_Socket) with
      Pre  => (not Client.Exists and not
                  WolfSSL.Is_Valid (Ssl) and not WolfSSL.Is_Valid (Ctx)),
      Post => (not Client.Exists and not WolfSSL.Is_Valid (Ssl) and
                  not WolfSSL.Is_Valid (Ctx));

end Tls_Client;
