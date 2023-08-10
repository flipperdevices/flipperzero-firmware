package body SPARK_Terminal is

   procedure Set_Exit_Status (Status : Exit_Status) is
   begin
      Ada.Command_Line.Set_Exit_Status (Status);
   end Set_Exit_Status;

   function Argument_Count return Natural is
   begin
      return Ada.Command_Line.Argument_Count;
   end Argument_Count;

   function Argument (Number : Positive) return String is
   begin
      return Ada.Command_Line.Argument (Number);
   end Argument;

end SPARK_Terminal;
