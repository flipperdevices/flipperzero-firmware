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

with Ada.Command_Line;

--  SPARK wrapper package around Ada.Command_Line and Interfaces.C
--  packages because these packages lack contracts in their specification
--  files that SPARK can use to verify the context in which
--  subprograms can safely be called.
package SPARK_Terminal with SPARK_Mode is

   subtype Exit_Status is Ada.Command_Line.Exit_Status;

   Exit_Status_Success : Exit_Status renames Ada.Command_Line.Success;
   Exit_Status_Failure : Exit_Status renames Ada.Command_Line.Failure;

   procedure Set_Exit_Status (Status : Exit_Status) with
      Global => null;

   function Argument_Count return Natural;

   function Argument (Number : Positive) return String with
      Pre => Number <= Argument_Count;

end SPARK_Terminal;
