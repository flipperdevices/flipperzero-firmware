<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE eagle SYSTEM "eagle.dtd">
<eagle version="7.3.0">
<drawing>
<settings>
<setting alwaysvectorfont="no"/>
<setting verticaltext="up"/>
</settings>
<grid distance="0.1" unitdist="inch" unit="inch" style="lines" multiple="1" display="no" altdistance="0.01" altunitdist="inch" altunit="inch"/>
<layers>
<layer number="1" name="Top" color="4" fill="1" visible="no" active="no"/>
<layer number="16" name="Bottom" color="1" fill="1" visible="no" active="no"/>
<layer number="17" name="Pads" color="2" fill="1" visible="no" active="no"/>
<layer number="18" name="Vias" color="2" fill="1" visible="no" active="no"/>
<layer number="19" name="Unrouted" color="6" fill="1" visible="no" active="no"/>
<layer number="20" name="Dimension" color="15" fill="1" visible="no" active="no"/>
<layer number="21" name="tPlace" color="7" fill="1" visible="no" active="no"/>
<layer number="22" name="bPlace" color="7" fill="1" visible="no" active="no"/>
<layer number="23" name="tOrigins" color="15" fill="1" visible="no" active="no"/>
<layer number="24" name="bOrigins" color="15" fill="1" visible="no" active="no"/>
<layer number="25" name="tNames" color="7" fill="1" visible="no" active="no"/>
<layer number="26" name="bNames" color="7" fill="1" visible="no" active="no"/>
<layer number="27" name="tValues" color="7" fill="1" visible="no" active="no"/>
<layer number="28" name="bValues" color="7" fill="1" visible="no" active="no"/>
<layer number="29" name="tStop" color="7" fill="3" visible="no" active="no"/>
<layer number="30" name="bStop" color="7" fill="6" visible="no" active="no"/>
<layer number="31" name="tCream" color="7" fill="4" visible="no" active="no"/>
<layer number="32" name="bCream" color="7" fill="5" visible="no" active="no"/>
<layer number="33" name="tFinish" color="6" fill="3" visible="no" active="no"/>
<layer number="34" name="bFinish" color="6" fill="6" visible="no" active="no"/>
<layer number="35" name="tGlue" color="7" fill="4" visible="no" active="no"/>
<layer number="36" name="bGlue" color="7" fill="5" visible="no" active="no"/>
<layer number="37" name="tTest" color="7" fill="1" visible="no" active="no"/>
<layer number="38" name="bTest" color="7" fill="1" visible="no" active="no"/>
<layer number="39" name="tKeepout" color="4" fill="11" visible="no" active="no"/>
<layer number="40" name="bKeepout" color="1" fill="11" visible="no" active="no"/>
<layer number="41" name="tRestrict" color="4" fill="10" visible="no" active="no"/>
<layer number="42" name="bRestrict" color="1" fill="10" visible="no" active="no"/>
<layer number="43" name="vRestrict" color="2" fill="10" visible="no" active="no"/>
<layer number="44" name="Drills" color="7" fill="1" visible="no" active="no"/>
<layer number="45" name="Holes" color="7" fill="1" visible="no" active="no"/>
<layer number="46" name="Milling" color="3" fill="1" visible="no" active="no"/>
<layer number="47" name="Measures" color="7" fill="1" visible="no" active="no"/>
<layer number="48" name="Document" color="7" fill="1" visible="no" active="no"/>
<layer number="49" name="Reference" color="7" fill="1" visible="no" active="no"/>
<layer number="51" name="tDocu" color="6" fill="1" visible="no" active="no"/>
<layer number="52" name="bDocu" color="7" fill="1" visible="no" active="no"/>
<layer number="90" name="Modules" color="5" fill="1" visible="yes" active="yes"/>
<layer number="91" name="Nets" color="2" fill="1" visible="yes" active="yes"/>
<layer number="92" name="Busses" color="1" fill="1" visible="yes" active="yes"/>
<layer number="93" name="Pins" color="2" fill="1" visible="no" active="yes"/>
<layer number="94" name="Symbols" color="4" fill="1" visible="yes" active="yes"/>
<layer number="95" name="Names" color="7" fill="1" visible="yes" active="yes"/>
<layer number="96" name="Values" color="7" fill="1" visible="yes" active="yes"/>
<layer number="97" name="Info" color="7" fill="1" visible="yes" active="yes"/>
<layer number="98" name="Guide" color="6" fill="1" visible="yes" active="yes"/>
</layers>
<schematic xreflabel="%F%N/%S.%C%R" xrefpart="/%S.%C%R">
<libraries>
<library name="ataradov_mcu">
<description>Alex Taradov Library (MCUs)</description>
<packages>
<package name="QFN24">
<wire x1="2.196" y1="1.492" x2="2.196" y2="2.196" width="0.127" layer="21"/>
<wire x1="2.196" y1="2.196" x2="1.492" y2="2.196" width="0.127" layer="21"/>
<wire x1="1.492" y1="2.196" x2="-1.492" y2="2.196" width="0.127" layer="51"/>
<wire x1="-1.492" y1="2.196" x2="-2.196" y2="1.492" width="0.127" layer="21"/>
<wire x1="-2.196" y1="1.492" x2="-2.196" y2="-1.492" width="0.127" layer="51"/>
<wire x1="-2.196" y1="-1.492" x2="-2.196" y2="-2.196" width="0.127" layer="21"/>
<wire x1="-2.196" y1="-2.196" x2="-1.492" y2="-2.196" width="0.127" layer="21"/>
<wire x1="-1.492" y1="-2.196" x2="1.492" y2="-2.196" width="0.127" layer="51"/>
<wire x1="1.492" y1="-2.196" x2="2.196" y2="-2.196" width="0.127" layer="21"/>
<wire x1="2.196" y1="-2.196" x2="2.196" y2="-1.492" width="0.127" layer="21"/>
<wire x1="2.196" y1="-1.492" x2="2.196" y2="1.492" width="0.127" layer="51"/>
<smd name="1" x="-1.85" y="1.25" dx="0.7" dy="0.28" layer="1" roundness="50" stop="no"/>
<smd name="2" x="-1.85" y="0.75" dx="0.7" dy="0.28" layer="1" stop="no"/>
<smd name="3" x="-1.85" y="0.25" dx="0.7" dy="0.28" layer="1" stop="no"/>
<smd name="4" x="-1.85" y="-0.25" dx="0.7" dy="0.28" layer="1" stop="no"/>
<smd name="5" x="-1.85" y="-0.75" dx="0.7" dy="0.28" layer="1" stop="no"/>
<smd name="6" x="-1.85" y="-1.25" dx="0.7" dy="0.28" layer="1" roundness="50" stop="no"/>
<smd name="7" x="-1.25" y="-1.85" dx="0.28" dy="0.7" layer="1" roundness="50" stop="no"/>
<smd name="8" x="-0.75" y="-1.85" dx="0.28" dy="0.7" layer="1" stop="no"/>
<smd name="9" x="-0.25" y="-1.85" dx="0.28" dy="0.7" layer="1" stop="no"/>
<smd name="10" x="0.25" y="-1.85" dx="0.28" dy="0.7" layer="1" stop="no"/>
<smd name="11" x="0.75" y="-1.85" dx="0.28" dy="0.7" layer="1" stop="no"/>
<smd name="12" x="1.25" y="-1.85" dx="0.28" dy="0.7" layer="1" roundness="50" stop="no"/>
<smd name="13" x="1.85" y="-1.25" dx="0.7" dy="0.28" layer="1" roundness="50" stop="no"/>
<smd name="14" x="1.85" y="-0.75" dx="0.7" dy="0.28" layer="1" stop="no"/>
<smd name="15" x="1.85" y="-0.25" dx="0.7" dy="0.28" layer="1" stop="no"/>
<smd name="16" x="1.85" y="0.25" dx="0.7" dy="0.28" layer="1" stop="no"/>
<smd name="17" x="1.85" y="0.75" dx="0.7" dy="0.28" layer="1" stop="no"/>
<smd name="18" x="1.85" y="1.25" dx="0.7" dy="0.28" layer="1" roundness="50" stop="no"/>
<smd name="19" x="1.25" y="1.85" dx="0.28" dy="0.7" layer="1" roundness="50" stop="no"/>
<smd name="20" x="0.75" y="1.85" dx="0.28" dy="0.7" layer="1" stop="no"/>
<smd name="21" x="0.25" y="1.85" dx="0.28" dy="0.7" layer="1" stop="no"/>
<smd name="22" x="-0.25" y="1.85" dx="0.28" dy="0.7" layer="1" stop="no"/>
<smd name="23" x="-0.75" y="1.85" dx="0.28" dy="0.7" layer="1" stop="no"/>
<smd name="24" x="-1.25" y="1.85" dx="0.28" dy="0.7" layer="1" roundness="50" stop="no"/>
<smd name="PAD" x="0" y="0" dx="2" dy="2" layer="1" stop="no"/>
<text x="0" y="2.54" size="1.27" layer="25" font="vector" ratio="10" align="bottom-center">&gt;NAME</text>
<rectangle x1="-1.024" y1="-1.024" x2="1.024" y2="1.024" layer="29"/>
<rectangle x1="-2.225" y1="1.075" x2="-1.475" y2="1.425" layer="29"/>
<rectangle x1="-2.225" y1="0.575" x2="-1.475" y2="0.925" layer="29"/>
<rectangle x1="-2.225" y1="0.075" x2="-1.475" y2="0.425" layer="29"/>
<rectangle x1="-2.225" y1="-0.425" x2="-1.475" y2="-0.075" layer="29"/>
<rectangle x1="-2.225" y1="-0.925" x2="-1.475" y2="-0.575" layer="29"/>
<rectangle x1="-2.225" y1="-1.425" x2="-1.475" y2="-1.075" layer="29"/>
<rectangle x1="-1.625" y1="-2.025" x2="-0.875" y2="-1.675" layer="29" rot="R90"/>
<rectangle x1="-1.125" y1="-2.025" x2="-0.375" y2="-1.675" layer="29" rot="R90"/>
<rectangle x1="-0.625" y1="-2.025" x2="0.125" y2="-1.675" layer="29" rot="R90"/>
<rectangle x1="-0.125" y1="-2.025" x2="0.625" y2="-1.675" layer="29" rot="R90"/>
<rectangle x1="0.375" y1="-2.025" x2="1.125" y2="-1.675" layer="29" rot="R90"/>
<rectangle x1="0.875" y1="-2.025" x2="1.625" y2="-1.675" layer="29" rot="R90"/>
<rectangle x1="1.475" y1="-1.425" x2="2.225" y2="-1.075" layer="29" rot="R180"/>
<rectangle x1="1.475" y1="-0.925" x2="2.225" y2="-0.575" layer="29" rot="R180"/>
<rectangle x1="1.475" y1="-0.425" x2="2.225" y2="-0.075" layer="29" rot="R180"/>
<rectangle x1="1.475" y1="0.075" x2="2.225" y2="0.425" layer="29" rot="R180"/>
<rectangle x1="1.475" y1="0.575" x2="2.225" y2="0.925" layer="29" rot="R180"/>
<rectangle x1="1.475" y1="1.075" x2="2.225" y2="1.425" layer="29" rot="R180"/>
<rectangle x1="0.875" y1="1.675" x2="1.625" y2="2.025" layer="29" rot="R270"/>
<rectangle x1="0.375" y1="1.675" x2="1.125" y2="2.025" layer="29" rot="R270"/>
<rectangle x1="-0.125" y1="1.675" x2="0.625" y2="2.025" layer="29" rot="R270"/>
<rectangle x1="-0.625" y1="1.675" x2="0.125" y2="2.025" layer="29" rot="R270"/>
<rectangle x1="-1.125" y1="1.675" x2="-0.375" y2="2.025" layer="29" rot="R270"/>
<rectangle x1="-1.625" y1="1.675" x2="-0.875" y2="2.025" layer="29" rot="R270"/>
</package>
</packages>
<symbols>
<symbol name="ATSAMDXXD">
<description>Atmel SAM D09D/D10D/D11D Cortex-M0+ microcontroller</description>
<wire x1="-12.7" y1="17.78" x2="12.7" y2="17.78" width="0.254" layer="94"/>
<wire x1="12.7" y1="17.78" x2="12.7" y2="-17.78" width="0.254" layer="94"/>
<wire x1="12.7" y1="-17.78" x2="-12.7" y2="-17.78" width="0.254" layer="94"/>
<wire x1="-12.7" y1="-17.78" x2="-12.7" y2="17.78" width="0.254" layer="94"/>
<pin name="PA28/RST" x="17.78" y="-5.08" length="middle" direction="pas" rot="R180"/>
<pin name="PA08" x="-17.78" y="0" length="middle" direction="pas"/>
<pin name="PA09" x="-17.78" y="-2.54" length="middle" direction="pas"/>
<pin name="PA14" x="-17.78" y="-10.16" length="middle" direction="pas"/>
<pin name="PA15" x="-17.78" y="-12.7" length="middle" direction="pas"/>
<pin name="PA05" x="-17.78" y="7.62" length="middle" direction="pas"/>
<pin name="PA04" x="-17.78" y="10.16" length="middle" direction="pas"/>
<pin name="PA02" x="-17.78" y="15.24" length="middle" direction="pas"/>
<text x="0" y="18.542" size="1.27" layer="95" align="bottom-center">&gt;NAME</text>
<text x="0" y="-19.812" size="1.27" layer="96" align="bottom-center">&gt;VALUE</text>
<pin name="PA30/SCK" x="17.78" y="-7.62" length="middle" direction="pas" rot="R180"/>
<pin name="VDD" x="17.78" y="15.24" length="middle" direction="pas" rot="R180"/>
<pin name="GND" x="17.78" y="-12.7" length="middle" direction="pas" rot="R180"/>
<pin name="PA25/DP" x="17.78" y="0" length="middle" direction="pas" rot="R180"/>
<pin name="PA24/DM" x="17.78" y="2.54" length="middle" direction="pas" rot="R180"/>
<pin name="PA31/SIO" x="17.78" y="-10.16" length="middle" direction="pas" rot="R180"/>
<pin name="PA03" x="-17.78" y="12.7" length="middle" direction="pas"/>
<pin name="PA06" x="-17.78" y="5.08" length="middle" direction="pas"/>
<pin name="PA07" x="-17.78" y="2.54" length="middle" direction="pas"/>
<pin name="PA10" x="-17.78" y="-5.08" length="middle" direction="pas"/>
<pin name="PA11" x="-17.78" y="-7.62" length="middle" direction="pas"/>
<pin name="PA16" x="-17.78" y="-15.24" length="middle" direction="pas"/>
<pin name="PA17" x="17.78" y="12.7" length="middle" direction="pas" rot="R180"/>
<pin name="PA22" x="17.78" y="10.16" length="middle" direction="pas" rot="R180"/>
<pin name="PA23" x="17.78" y="7.62" length="middle" direction="pas" rot="R180"/>
<pin name="PA27" x="17.78" y="5.08" length="middle" direction="pas" rot="R180"/>
<pin name="PAD" x="17.78" y="-15.24" length="middle" direction="pas" rot="R180"/>
</symbol>
</symbols>
<devicesets>
<deviceset name="ATSAMD11D" prefix="IC">
<gates>
<gate name="G$1" symbol="ATSAMDXXD" x="0" y="0"/>
</gates>
<devices>
<device name="" package="QFN24">
<connects>
<connect gate="G$1" pin="GND" pad="23"/>
<connect gate="G$1" pin="PA02" pad="1"/>
<connect gate="G$1" pin="PA03" pad="2"/>
<connect gate="G$1" pin="PA04" pad="3"/>
<connect gate="G$1" pin="PA05" pad="4"/>
<connect gate="G$1" pin="PA06" pad="5"/>
<connect gate="G$1" pin="PA07" pad="6"/>
<connect gate="G$1" pin="PA08" pad="7"/>
<connect gate="G$1" pin="PA09" pad="8"/>
<connect gate="G$1" pin="PA10" pad="9"/>
<connect gate="G$1" pin="PA11" pad="10"/>
<connect gate="G$1" pin="PA14" pad="11"/>
<connect gate="G$1" pin="PA15" pad="12"/>
<connect gate="G$1" pin="PA16" pad="13"/>
<connect gate="G$1" pin="PA17" pad="14"/>
<connect gate="G$1" pin="PA22" pad="15"/>
<connect gate="G$1" pin="PA23" pad="16"/>
<connect gate="G$1" pin="PA24/DM" pad="21"/>
<connect gate="G$1" pin="PA25/DP" pad="22"/>
<connect gate="G$1" pin="PA27" pad="17"/>
<connect gate="G$1" pin="PA28/RST" pad="18"/>
<connect gate="G$1" pin="PA30/SCK" pad="19"/>
<connect gate="G$1" pin="PA31/SIO" pad="20"/>
<connect gate="G$1" pin="PAD" pad="PAD"/>
<connect gate="G$1" pin="VDD" pad="24"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
</devicesets>
</library>
<library name="ataradov_pwr">
<description>Alex Taradov Library (Power Symbols)</description>
<packages>
</packages>
<symbols>
<symbol name="+3V3">
<wire x1="-1.905" y1="0" x2="1.905" y2="0" width="0.254" layer="94"/>
<text x="0" y="0.762" size="1.27" layer="96" align="bottom-center">&gt;VALUE</text>
<pin name="+3V3" x="0" y="-2.54" visible="off" length="short" direction="sup" rot="R90"/>
</symbol>
<symbol name="GND">
<wire x1="-1.905" y1="0" x2="1.905" y2="0" width="0.254" layer="94"/>
<text x="0" y="-2.032" size="1.27" layer="96" align="bottom-center">&gt;VALUE</text>
<pin name="GND" x="0" y="2.54" visible="off" length="short" direction="sup" rot="R270"/>
</symbol>
<symbol name="V_USB">
<wire x1="-1.905" y1="0" x2="1.905" y2="0" width="0.254" layer="94"/>
<text x="0" y="0.762" size="1.27" layer="96" align="bottom-center">&gt;VALUE</text>
<pin name="V_USB" x="0" y="-2.54" visible="off" length="short" direction="sup" rot="R90"/>
</symbol>
</symbols>
<devicesets>
<deviceset name="+3V3" prefix="P_">
<gates>
<gate name="1" symbol="+3V3" x="0" y="0"/>
</gates>
<devices>
<device name="">
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
<deviceset name="GND" prefix="P_">
<gates>
<gate name="1" symbol="GND" x="0" y="0"/>
</gates>
<devices>
<device name="">
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
<deviceset name="V_USB" prefix="P_">
<gates>
<gate name="1" symbol="V_USB" x="0" y="0"/>
</gates>
<devices>
<device name="">
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
</devicesets>
</library>
<library name="ataradov_conn">
<description>Alex Taradov Library (Connectors)</description>
<packages>
<package name="HEADER-5X2-1.27MM">
<wire x1="-3.81" y1="1.905" x2="3.81" y2="1.905" width="0.127" layer="21"/>
<wire x1="3.81" y1="1.905" x2="3.81" y2="-1.905" width="0.127" layer="21"/>
<wire x1="3.81" y1="-1.905" x2="-3.175" y2="-1.905" width="0.127" layer="21"/>
<wire x1="-3.175" y1="-1.905" x2="-3.81" y2="-1.27" width="0.127" layer="21"/>
<wire x1="-3.81" y1="-1.27" x2="-3.81" y2="1.905" width="0.127" layer="21"/>
<pad name="1" x="-2.54" y="-0.635" drill="0.6096" diameter="1.016"/>
<pad name="2" x="-2.54" y="0.635" drill="0.6096" diameter="1.016"/>
<pad name="3" x="-1.27" y="-0.635" drill="0.6096" diameter="1.016"/>
<pad name="4" x="-1.27" y="0.635" drill="0.6096" diameter="1.016"/>
<pad name="5" x="0" y="-0.635" drill="0.6096" diameter="1.016"/>
<pad name="6" x="0" y="0.635" drill="0.6096" diameter="1.016"/>
<pad name="7" x="1.27" y="-0.635" drill="0.6096" diameter="1.016"/>
<pad name="8" x="1.27" y="0.635" drill="0.6096" diameter="1.016"/>
<pad name="9" x="2.54" y="-0.635" drill="0.6096" diameter="1.016"/>
<pad name="10" x="2.54" y="0.635" drill="0.6096" diameter="1.016"/>
<text x="0" y="2.54" size="1.27" layer="25" font="vector" align="bottom-center">&gt;NAME</text>
</package>
<package name="HEADER-5X2-2.54MM">
<wire x1="-6.35" y1="0" x2="-3.81" y2="0" width="0.127" layer="21"/>
<wire x1="-3.81" y1="0" x2="-3.81" y2="-2.54" width="0.127" layer="21"/>
<wire x1="-6.35" y1="2.54" x2="6.35" y2="2.54" width="0.127" layer="21"/>
<wire x1="6.35" y1="2.54" x2="6.35" y2="-2.54" width="0.127" layer="21"/>
<wire x1="6.35" y1="-2.54" x2="-6.35" y2="-2.54" width="0.127" layer="21"/>
<wire x1="-6.35" y1="-2.54" x2="-6.35" y2="2.54" width="0.127" layer="21"/>
<pad name="1" x="-5.08" y="-1.27" drill="1.016" shape="square"/>
<pad name="2" x="-5.08" y="1.27" drill="1.016"/>
<pad name="3" x="-2.54" y="-1.27" drill="1.016"/>
<pad name="4" x="-2.54" y="1.27" drill="1.016"/>
<pad name="5" x="0" y="-1.27" drill="1.016"/>
<pad name="6" x="0" y="1.27" drill="1.016"/>
<pad name="7" x="2.54" y="-1.27" drill="1.016"/>
<pad name="8" x="2.54" y="1.27" drill="1.016"/>
<pad name="9" x="5.08" y="-1.27" drill="1.016"/>
<pad name="10" x="5.08" y="1.27" drill="1.016"/>
<text x="0" y="3.175" size="1.27" layer="25" font="vector" align="bottom-center">&gt;NAME</text>
</package>
<package name="USB-B-MICRO-SMT-SHELL-TH">
<pad name="M1" x="2.15" y="-3.6" drill="1.143"/>
<pad name="M2" x="2.15" y="3.6" drill="1.143"/>
<smd name="1" x="4.825" y="1.3" dx="1.35" dy="0.4" layer="1" stop="no"/>
<smd name="2" x="4.825" y="0.65" dx="1.35" dy="0.4" layer="1" stop="no"/>
<smd name="3" x="4.825" y="0" dx="1.35" dy="0.4" layer="1" stop="no"/>
<smd name="4" x="4.825" y="-0.65" dx="1.35" dy="0.4" layer="1" stop="no"/>
<smd name="5" x="4.825" y="-1.3" dx="1.35" dy="0.4" layer="1" stop="no"/>
<text x="3.175" y="0" size="1.27" layer="25" font="vector" rot="R90" align="bottom-center">&gt;NAME</text>
<rectangle x1="4.1" y1="1.05" x2="5.55" y2="1.55" layer="29"/>
<pad name="M3" x="5.05" y="2.425" drill="0.635" shape="long"/>
<pad name="M4" x="5.05" y="-2.425" drill="0.635" shape="long"/>
<rectangle x1="4.1" y1="0.4" x2="5.55" y2="0.9" layer="29"/>
<rectangle x1="4.1" y1="-0.25" x2="5.55" y2="0.25" layer="29"/>
<rectangle x1="4.1" y1="-0.9" x2="5.55" y2="-0.4" layer="29"/>
<rectangle x1="4.1" y1="-1.55" x2="5.55" y2="-1.05" layer="29"/>
<wire x1="-0.5" y1="4" x2="-0.6" y2="3.8" width="0.1524" layer="21"/>
<wire x1="-0.6" y1="3.8" x2="0" y2="3.5" width="0.1524" layer="21"/>
<wire x1="0" y1="3.5" x2="0" y2="3.1" width="0.1524" layer="21"/>
<wire x1="0" y1="3.1" x2="-0.6" y2="3.1" width="0.1524" layer="21"/>
<wire x1="-0.6" y1="3.1" x2="-0.6" y2="-3.1" width="0.1524" layer="21"/>
<wire x1="-0.6" y1="-3.1" x2="0" y2="-3.1" width="0.1524" layer="21"/>
<wire x1="0" y1="-3.1" x2="0" y2="-3.5" width="0.1524" layer="21"/>
<wire x1="0" y1="-3.5" x2="-0.6" y2="-3.8" width="0.1524" layer="21"/>
<wire x1="-0.6" y1="-3.8" x2="-0.5" y2="-4" width="0.1524" layer="21"/>
<wire x1="-0.5" y1="-4" x2="0" y2="-3.75" width="0.1524" layer="21"/>
<wire x1="0" y1="-3.75" x2="5" y2="-3.75" width="0.1524" layer="21"/>
<wire x1="5" y1="-3.75" x2="5" y2="-3.16" width="0.1524" layer="21"/>
<wire x1="-0.5" y1="4" x2="0" y2="3.75" width="0.1524" layer="21"/>
<wire x1="0" y1="3.75" x2="5" y2="3.75" width="0.1524" layer="21"/>
<wire x1="5" y1="3.75" x2="5" y2="3.16" width="0.1524" layer="21"/>
</package>
<package name="PIN-TH">
<wire x1="-0.635" y1="1.27" x2="0.635" y2="1.27" width="0.1524" layer="21"/>
<wire x1="0.635" y1="1.27" x2="1.27" y2="0.635" width="0.1524" layer="21"/>
<wire x1="1.27" y1="0.635" x2="1.27" y2="-0.635" width="0.1524" layer="21"/>
<wire x1="1.27" y1="-0.635" x2="0.635" y2="-1.27" width="0.1524" layer="21"/>
<wire x1="-1.27" y1="0.635" x2="-1.27" y2="-0.635" width="0.1524" layer="21"/>
<wire x1="-0.635" y1="1.27" x2="-1.27" y2="0.635" width="0.1524" layer="21"/>
<wire x1="-1.27" y1="-0.635" x2="-0.635" y2="-1.27" width="0.1524" layer="21"/>
<wire x1="0.635" y1="-1.27" x2="-0.635" y2="-1.27" width="0.1524" layer="21"/>
<pad name="1" x="0" y="0" drill="1.016"/>
<text x="0" y="1.524" size="1.27" layer="25" font="vector" align="bottom-center">&gt;NAME</text>
<text x="0" y="-2.794" size="1.27" layer="21" font="vector" align="bottom-center">&gt;VALUE</text>
</package>
<package name="HEADER-5X2-1.27MM-SHR">
<wire x1="-6.35" y1="2.794" x2="6.35" y2="2.794" width="0.127" layer="21"/>
<wire x1="6.35" y1="2.794" x2="6.35" y2="-2.794" width="0.127" layer="21"/>
<wire x1="6.35" y1="-2.794" x2="-6.35" y2="-2.794" width="0.127" layer="21"/>
<wire x1="-6.35" y1="-2.794" x2="-6.35" y2="2.794" width="0.127" layer="21"/>
<pad name="1" x="-2.54" y="-0.635" drill="0.6604" diameter="1.0668"/>
<pad name="2" x="-2.54" y="0.635" drill="0.6604" diameter="1.0668"/>
<pad name="3" x="-1.27" y="-0.635" drill="0.6604" diameter="1.0668"/>
<pad name="4" x="-1.27" y="0.635" drill="0.6604" diameter="1.0668"/>
<pad name="5" x="0" y="-0.635" drill="0.6604" diameter="1.0668"/>
<pad name="6" x="0" y="0.635" drill="0.6604" diameter="1.0668"/>
<pad name="7" x="1.27" y="-0.635" drill="0.6604" diameter="1.0668"/>
<pad name="8" x="1.27" y="0.635" drill="0.6604" diameter="1.0668"/>
<pad name="9" x="2.54" y="-0.635" drill="0.6604" diameter="1.0668"/>
<pad name="10" x="2.54" y="0.635" drill="0.6604" diameter="1.0668"/>
<text x="0" y="3.302" size="1.27" layer="25" font="vector" align="bottom-center">&gt;NAME</text>
<circle x="-5.334" y="-1.778" radius="0.381" width="0.254" layer="21"/>
</package>
<package name="HEADER-5X2-2.54MM-SHR-RA">
<description>10-pin right angle shrouded header</description>
<wire x1="10.16" y1="2.794" x2="10.16" y2="10.922" width="0.127" layer="21"/>
<wire x1="10.16" y1="10.922" x2="1.778" y2="10.922" width="0.127" layer="21"/>
<wire x1="1.778" y1="10.922" x2="1.778" y2="4.572" width="0.127" layer="21"/>
<wire x1="1.778" y1="4.572" x2="-1.778" y2="4.572" width="0.127" layer="21"/>
<wire x1="-1.778" y1="4.572" x2="-1.778" y2="10.922" width="0.127" layer="21"/>
<wire x1="1.778" y1="10.922" x2="-1.778" y2="10.922" width="0.127" layer="21"/>
<wire x1="-1.778" y1="10.922" x2="-10.16" y2="10.922" width="0.127" layer="21"/>
<wire x1="-10.16" y1="10.922" x2="-10.16" y2="2.794" width="0.127" layer="21"/>
<wire x1="10.16" y1="2.794" x2="-10.16" y2="2.794" width="0.127" layer="21"/>
<pad name="1" x="-5.08" y="-1.27" drill="1" diameter="1.4224" shape="square"/>
<pad name="2" x="-5.08" y="1.27" drill="1" diameter="1.4224"/>
<pad name="3" x="-2.54" y="-1.27" drill="1" diameter="1.4224"/>
<pad name="4" x="-2.54" y="1.27" drill="1" diameter="1.4224"/>
<pad name="5" x="0" y="-1.27" drill="1" diameter="1.4224"/>
<pad name="6" x="0" y="1.27" drill="1" diameter="1.4224"/>
<pad name="7" x="2.54" y="-1.27" drill="1" diameter="1.4224"/>
<pad name="8" x="2.54" y="1.27" drill="1" diameter="1.4224"/>
<pad name="9" x="5.08" y="-1.27" drill="1" diameter="1.4224"/>
<pad name="10" x="5.08" y="1.27" drill="1" diameter="1.4224"/>
<text x="-8.89" y="-0.508" size="1.27" layer="25" font="vector" align="bottom-center">&gt;NAME</text>
<wire x1="-8.89" y1="10.16" x2="-6.35" y2="10.16" width="0.127" layer="21"/>
<wire x1="-6.35" y1="10.16" x2="-7.62" y2="8.89" width="0.127" layer="21"/>
<wire x1="-7.62" y1="8.89" x2="-8.89" y2="10.16" width="0.127" layer="21"/>
</package>
<package name="PIN-TH-LARGE">
<wire x1="-0.635" y1="1.651" x2="0.635" y2="1.651" width="0.1524" layer="21"/>
<wire x1="0.635" y1="1.651" x2="1.651" y2="0.635" width="0.1524" layer="21"/>
<wire x1="1.651" y1="0.635" x2="1.651" y2="-0.635" width="0.1524" layer="21"/>
<wire x1="1.651" y1="-0.635" x2="0.635" y2="-1.651" width="0.1524" layer="21"/>
<wire x1="-1.651" y1="0.635" x2="-1.651" y2="-0.635" width="0.1524" layer="21"/>
<wire x1="-0.635" y1="1.651" x2="-1.651" y2="0.635" width="0.1524" layer="21"/>
<wire x1="-1.651" y1="-0.635" x2="-0.635" y2="-1.651" width="0.1524" layer="21"/>
<wire x1="0.635" y1="-1.651" x2="-0.635" y2="-1.651" width="0.1524" layer="21"/>
<pad name="1" x="0" y="0" drill="1.778"/>
<text x="0" y="2.032" size="1.27" layer="25" font="vector" align="bottom-center">&gt;NAME</text>
<text x="0" y="-3.302" size="1.27" layer="21" font="vector" align="bottom-center">&gt;VALUE</text>
</package>
<package name="PIN-TH-SMALL">
<wire x1="-0.635" y1="1.27" x2="0.635" y2="1.27" width="0.1524" layer="21"/>
<wire x1="0.635" y1="1.27" x2="1.27" y2="0.635" width="0.1524" layer="21"/>
<wire x1="1.27" y1="0.635" x2="1.27" y2="-0.635" width="0.1524" layer="21"/>
<wire x1="1.27" y1="-0.635" x2="0.635" y2="-1.27" width="0.1524" layer="21"/>
<wire x1="-1.27" y1="0.635" x2="-1.27" y2="-0.635" width="0.1524" layer="21"/>
<wire x1="-0.635" y1="1.27" x2="-1.27" y2="0.635" width="0.1524" layer="21"/>
<wire x1="-1.27" y1="-0.635" x2="-0.635" y2="-1.27" width="0.1524" layer="21"/>
<wire x1="0.635" y1="-1.27" x2="-0.635" y2="-1.27" width="0.1524" layer="21"/>
<pad name="1" x="0" y="0" drill="0.9"/>
<text x="0" y="1.524" size="1.27" layer="25" font="vector" align="bottom-center">&gt;NAME</text>
<text x="0" y="-2.794" size="1.27" layer="21" font="vector" align="bottom-center">&gt;VALUE</text>
</package>
</packages>
<symbols>
<symbol name="HEADER-5X2">
<wire x1="-5.08" y1="7.62" x2="-5.08" y2="-7.62" width="0.254" layer="94"/>
<wire x1="-5.08" y1="-7.62" x2="5.08" y2="-7.62" width="0.254" layer="94"/>
<wire x1="5.08" y1="-7.62" x2="5.08" y2="7.62" width="0.254" layer="94"/>
<wire x1="5.08" y1="7.62" x2="-5.08" y2="7.62" width="0.254" layer="94"/>
<pin name="1" x="-7.62" y="5.08" length="short" direction="pas"/>
<pin name="2" x="7.62" y="5.08" length="short" direction="pas" rot="R180"/>
<pin name="3" x="-7.62" y="2.54" length="short" direction="pas"/>
<pin name="4" x="7.62" y="2.54" length="short" direction="pas" rot="R180"/>
<pin name="5" x="-7.62" y="0" length="short" direction="pas"/>
<pin name="6" x="7.62" y="0" length="short" direction="pas" rot="R180"/>
<pin name="7" x="-7.62" y="-2.54" length="short" direction="pas"/>
<pin name="8" x="7.62" y="-2.54" length="short" direction="pas" rot="R180"/>
<pin name="9" x="-7.62" y="-5.08" length="short" direction="pas"/>
<pin name="10" x="7.62" y="-5.08" length="short" direction="pas" rot="R180"/>
<text x="0" y="8.382" size="1.27" layer="95" align="bottom-center">&gt;NAME</text>
</symbol>
<symbol name="MICRO-USB-5">
<wire x1="-5.08" y1="7.62" x2="-5.08" y2="-10.16" width="0.254" layer="94"/>
<text x="0.762" y="8.382" size="1.27" layer="95" align="bottom-center">&gt;NAME</text>
<pin name="VBUS" x="-7.62" y="5.08" visible="pin" length="short" direction="pas"/>
<pin name="DM" x="-7.62" y="2.54" visible="pin" length="short" direction="pas"/>
<pin name="DP" x="-7.62" y="0" visible="pin" length="short" direction="pas"/>
<pin name="ID" x="-7.62" y="-2.54" visible="pin" length="short" direction="pas"/>
<pin name="GND" x="-7.62" y="-5.08" visible="pin" length="short" direction="pas"/>
<wire x1="7.62" y1="7.62" x2="7.62" y2="-10.16" width="0.254" layer="94"/>
<wire x1="-5.08" y1="7.62" x2="7.62" y2="7.62" width="0.254" layer="94"/>
<wire x1="-5.08" y1="-10.16" x2="7.62" y2="-10.16" width="0.254" layer="94"/>
<pin name="SHIELD" x="-7.62" y="-7.62" visible="pin" length="short" direction="pas"/>
<text x="0.762" y="-12.192" size="1.27" layer="95" align="bottom-center">&gt;VALUE</text>
</symbol>
<symbol name="CONN-SINGLE">
<wire x1="-2.54" y1="0.762" x2="0.127" y2="0" width="0.254" layer="94"/>
<wire x1="0.127" y1="0" x2="-2.4765" y2="-0.762" width="0.254" layer="94"/>
<pin name="1" x="-2.54" y="0" visible="off" length="short" direction="pas" swaplevel="1"/>
<text x="1.27" y="-0.508" size="1.27" layer="95">&gt;NAME</text>
<text x="6.096" y="-0.508" size="1.27" layer="96">&gt;VALUE</text>
</symbol>
</symbols>
<devicesets>
<deviceset name="HEADER-5X2" prefix="J">
<gates>
<gate name="G$1" symbol="HEADER-5X2" x="0" y="0"/>
</gates>
<devices>
<device name="-TH-1.27" package="HEADER-5X2-1.27MM">
<connects>
<connect gate="G$1" pin="1" pad="1"/>
<connect gate="G$1" pin="10" pad="10"/>
<connect gate="G$1" pin="2" pad="2"/>
<connect gate="G$1" pin="3" pad="3"/>
<connect gate="G$1" pin="4" pad="4"/>
<connect gate="G$1" pin="5" pad="5"/>
<connect gate="G$1" pin="6" pad="6"/>
<connect gate="G$1" pin="7" pad="7"/>
<connect gate="G$1" pin="8" pad="8"/>
<connect gate="G$1" pin="9" pad="9"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
<device name="-TH-1.27-SHR" package="HEADER-5X2-1.27MM-SHR">
<connects>
<connect gate="G$1" pin="1" pad="1"/>
<connect gate="G$1" pin="10" pad="10"/>
<connect gate="G$1" pin="2" pad="2"/>
<connect gate="G$1" pin="3" pad="3"/>
<connect gate="G$1" pin="4" pad="4"/>
<connect gate="G$1" pin="5" pad="5"/>
<connect gate="G$1" pin="6" pad="6"/>
<connect gate="G$1" pin="7" pad="7"/>
<connect gate="G$1" pin="8" pad="8"/>
<connect gate="G$1" pin="9" pad="9"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
<device name="-TH-2.54-SHR-RA" package="HEADER-5X2-2.54MM-SHR-RA">
<connects>
<connect gate="G$1" pin="1" pad="1"/>
<connect gate="G$1" pin="10" pad="10"/>
<connect gate="G$1" pin="2" pad="2"/>
<connect gate="G$1" pin="3" pad="3"/>
<connect gate="G$1" pin="4" pad="4"/>
<connect gate="G$1" pin="5" pad="5"/>
<connect gate="G$1" pin="6" pad="6"/>
<connect gate="G$1" pin="7" pad="7"/>
<connect gate="G$1" pin="8" pad="8"/>
<connect gate="G$1" pin="9" pad="9"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
<device name="-TH-2.54" package="HEADER-5X2-2.54MM">
<connects>
<connect gate="G$1" pin="1" pad="1"/>
<connect gate="G$1" pin="10" pad="10"/>
<connect gate="G$1" pin="2" pad="2"/>
<connect gate="G$1" pin="3" pad="3"/>
<connect gate="G$1" pin="4" pad="4"/>
<connect gate="G$1" pin="5" pad="5"/>
<connect gate="G$1" pin="6" pad="6"/>
<connect gate="G$1" pin="7" pad="7"/>
<connect gate="G$1" pin="8" pad="8"/>
<connect gate="G$1" pin="9" pad="9"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
<deviceset name="USB-B-MICRO" prefix="J" uservalue="yes">
<gates>
<gate name="G$1" symbol="MICRO-USB-5" x="0" y="0"/>
</gates>
<devices>
<device name="-SMT" package="USB-B-MICRO-SMT-SHELL-TH">
<connects>
<connect gate="G$1" pin="DM" pad="2"/>
<connect gate="G$1" pin="DP" pad="3"/>
<connect gate="G$1" pin="GND" pad="5"/>
<connect gate="G$1" pin="ID" pad="4"/>
<connect gate="G$1" pin="SHIELD" pad="M1 M2 M3 M4" route="any"/>
<connect gate="G$1" pin="VBUS" pad="1"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
<deviceset name="CONN-SINGLE" prefix="J" uservalue="yes">
<gates>
<gate name="G$1" symbol="CONN-SINGLE" x="0" y="0"/>
</gates>
<devices>
<device name="-TH" package="PIN-TH">
<connects>
<connect gate="G$1" pin="1" pad="1"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
<device name="-TH-LARGE" package="PIN-TH-LARGE">
<connects>
<connect gate="G$1" pin="1" pad="1"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
<device name="-TH-SMALL" package="PIN-TH-SMALL">
<connects>
<connect gate="G$1" pin="1" pad="1"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
</devicesets>
</library>
<library name="ataradov_rlc">
<description>Alex Taradov Library (R, L and C)</description>
<packages>
<package name="SMD0508">
<wire x1="-1" y1="1.25" x2="1" y2="1.25" width="0.1" layer="21"/>
<wire x1="1" y1="1.25" x2="1" y2="-1.25" width="0.1" layer="21"/>
<wire x1="1" y1="-1.25" x2="-1" y2="-1.25" width="0.1" layer="21"/>
<wire x1="-1" y1="-1.25" x2="-1" y2="1.25" width="0.1" layer="21"/>
<smd name="1" x="-0.5" y="0" dx="2" dy="0.5" layer="1" rot="R90"/>
<smd name="2" x="0.5" y="0" dx="2" dy="0.5" layer="1" rot="R90"/>
<text x="0" y="1.524" size="1.27" layer="25" font="vector" align="bottom-center">&gt;NAME</text>
</package>
<package name="SMD0603">
<wire x1="-1.5" y1="0.75" x2="1.5" y2="0.75" width="0.1" layer="21"/>
<wire x1="1.5" y1="0.75" x2="1.5" y2="-0.75" width="0.1" layer="21"/>
<wire x1="1.5" y1="-0.75" x2="-1.5" y2="-0.75" width="0.1" layer="21"/>
<wire x1="-1.5" y1="-0.75" x2="-1.5" y2="0.75" width="0.1" layer="21"/>
<smd name="1" x="-0.8" y="0" dx="0.9" dy="0.9" layer="1"/>
<smd name="2" x="0.8" y="0" dx="0.9" dy="0.9" layer="1"/>
<text x="0" y="1.016" size="1.27" layer="25" font="vector" align="bottom-center">&gt;NAME</text>
<text x="0" y="-0.254" size="0.508" layer="27" font="vector" align="bottom-center">&gt;VALUE</text>
</package>
<package name="SMD0612">
<wire x1="-1.8" y1="1.4" x2="1.8" y2="1.4" width="0.1" layer="21"/>
<wire x1="1.8" y1="1.4" x2="1.8" y2="-1.4" width="0.1" layer="21"/>
<wire x1="1.8" y1="-1.4" x2="-1.8" y2="-1.4" width="0.1" layer="21"/>
<wire x1="-1.8" y1="-1.4" x2="-1.8" y2="1.4" width="0.1" layer="21"/>
<smd name="1" x="0" y="0.8" dx="3.2" dy="0.8" layer="1"/>
<smd name="2" x="0" y="-0.8" dx="3.2" dy="0.8" layer="1"/>
<text x="0" y="1.778" size="1.27" layer="25" font="vector" align="bottom-center">&gt;NAME</text>
</package>
<package name="SMD1206">
<wire x1="-2.5" y1="1.5" x2="2.5" y2="1.5" width="0.1" layer="21"/>
<wire x1="2.5" y1="1.5" x2="2.5" y2="-1.5" width="0.1" layer="21"/>
<wire x1="2.5" y1="-1.5" x2="-2.5" y2="-1.5" width="0.1" layer="21"/>
<wire x1="-2.5" y1="-1.5" x2="-2.5" y2="1.5" width="0.1" layer="21"/>
<smd name="1" x="-1.4" y="0" dx="1.6" dy="2" layer="1"/>
<smd name="2" x="1.4" y="0" dx="1.6" dy="2" layer="1"/>
<text x="0" y="1.778" size="1.27" layer="25" font="vector" align="bottom-center">&gt;NAME</text>
</package>
<package name="SMD2512">
<wire x1="-5.5" y1="2.5" x2="5.5" y2="2.5" width="0.1" layer="21"/>
<wire x1="5.5" y1="2.5" x2="5.5" y2="-2.5" width="0.1" layer="21"/>
<wire x1="5.5" y1="-2.5" x2="-5.5" y2="-2.5" width="0.1" layer="21"/>
<wire x1="-5.5" y1="-2.5" x2="-5.5" y2="2.5" width="0.1" layer="21"/>
<smd name="1" x="-3.25" y="0" dx="3.5" dy="3" layer="1" rot="R90"/>
<smd name="2" x="3.25" y="0" dx="3.5" dy="3" layer="1" rot="R90"/>
<text x="0" y="2.794" size="1.27" layer="25" font="vector" align="bottom-center">&gt;NAME</text>
</package>
<package name="SMD0402">
<wire x1="-1" y1="0.5" x2="1" y2="0.5" width="0.1" layer="21"/>
<wire x1="1" y1="0.5" x2="1" y2="-0.5" width="0.1" layer="21"/>
<wire x1="1" y1="-0.5" x2="-1" y2="-0.5" width="0.1" layer="21"/>
<wire x1="-1" y1="-0.5" x2="-1" y2="0.5" width="0.1" layer="21"/>
<smd name="1" x="-0.5" y="0" dx="0.6" dy="0.6" layer="1"/>
<smd name="2" x="0.5" y="0" dx="0.6" dy="0.6" layer="1"/>
<text x="0" y="0.762" size="1.27" layer="25" font="vector" align="bottom-center">&gt;NAME</text>
<text x="0" y="-0.254" size="0.508" layer="27" font="vector" align="bottom-center">&gt;VALUE</text>
</package>
<package name="SMD0805">
<wire x1="-2" y1="1" x2="2" y2="1" width="0.1" layer="21"/>
<wire x1="2" y1="1" x2="2" y2="-1" width="0.1" layer="21"/>
<wire x1="2" y1="-1" x2="-2" y2="-1" width="0.1" layer="21"/>
<wire x1="-2" y1="-1" x2="-2" y2="1" width="0.1" layer="21"/>
<smd name="1" x="-0.95" y="0" dx="1.3" dy="1.5" layer="1"/>
<smd name="2" x="0.95" y="0" dx="1.3" dy="1.5" layer="1"/>
<text x="0" y="1.27" size="1.27" layer="25" font="vector" align="bottom-center">&gt;NAME</text>
<text x="0" y="-0.254" size="0.508" layer="27" font="vector" align="bottom-center">&gt;VALUE</text>
</package>
</packages>
<symbols>
<symbol name="C">
<rectangle x1="-1.524" y1="-0.254" x2="2.54" y2="0.254" layer="94" rot="R90"/>
<rectangle x1="-2.54" y1="-0.254" x2="1.524" y2="0.254" layer="94" rot="R90"/>
<wire x1="-2.54" y1="0" x2="-0.762" y2="0" width="0.1524" layer="94"/>
<wire x1="2.54" y1="0" x2="0.762" y2="0" width="0.1524" layer="94"/>
<pin name="1" x="-2.54" y="0" visible="off" length="point" direction="pas" swaplevel="1"/>
<pin name="2" x="2.54" y="0" visible="off" length="point" direction="pas" swaplevel="1" rot="R180"/>
<text x="0" y="2.413" size="1.27" layer="95" align="bottom-center">&gt;NAME</text>
<text x="0" y="-3.683" size="1.27" layer="96" align="bottom-center">&gt;VALUE</text>
</symbol>
<symbol name="R">
<wire x1="-2.54" y1="-1.016" x2="-2.54" y2="1.016" width="0.254" layer="94"/>
<pin name="1" x="-5.08" y="0" visible="off" length="short" direction="pas" swaplevel="1"/>
<pin name="2" x="5.08" y="0" visible="off" length="short" direction="pas" swaplevel="1" rot="R180"/>
<text x="0" y="1.524" size="1.27" layer="95" align="bottom-center">&gt;NAME</text>
<text x="0" y="0" size="1.016" layer="96" align="center">&gt;VALUE</text>
<wire x1="2.54" y1="-1.016" x2="2.54" y2="1.016" width="0.254" layer="94"/>
<wire x1="-2.54" y1="1.016" x2="2.54" y2="1.016" width="0.254" layer="94"/>
<wire x1="-2.54" y1="-1.016" x2="2.54" y2="-1.016" width="0.254" layer="94"/>
</symbol>
</symbols>
<devicesets>
<deviceset name="C" prefix="C" uservalue="yes">
<gates>
<gate name="G$1" symbol="C" x="0" y="0"/>
</gates>
<devices>
<device name="-0402" package="SMD0402">
<connects>
<connect gate="G$1" pin="1" pad="1"/>
<connect gate="G$1" pin="2" pad="2"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
<device name="-0508" package="SMD0508">
<connects>
<connect gate="G$1" pin="1" pad="1"/>
<connect gate="G$1" pin="2" pad="2"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
<device name="-0603" package="SMD0603">
<connects>
<connect gate="G$1" pin="1" pad="1"/>
<connect gate="G$1" pin="2" pad="2"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
<device name="-0612" package="SMD0612">
<connects>
<connect gate="G$1" pin="1" pad="1"/>
<connect gate="G$1" pin="2" pad="2"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
<device name="-0805" package="SMD0805">
<connects>
<connect gate="G$1" pin="1" pad="1"/>
<connect gate="G$1" pin="2" pad="2"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
<device name="-1206" package="SMD1206">
<connects>
<connect gate="G$1" pin="1" pad="1"/>
<connect gate="G$1" pin="2" pad="2"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
<device name="-2512" package="SMD2512">
<connects>
<connect gate="G$1" pin="1" pad="1"/>
<connect gate="G$1" pin="2" pad="2"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
<deviceset name="R" prefix="R" uservalue="yes">
<gates>
<gate name="G$1" symbol="R" x="0" y="0"/>
</gates>
<devices>
<device name="-0402" package="SMD0402">
<connects>
<connect gate="G$1" pin="1" pad="1"/>
<connect gate="G$1" pin="2" pad="2"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
<device name="-0508" package="SMD0508">
<connects>
<connect gate="G$1" pin="1" pad="1"/>
<connect gate="G$1" pin="2" pad="2"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
<device name="-0603" package="SMD0603">
<connects>
<connect gate="G$1" pin="1" pad="1"/>
<connect gate="G$1" pin="2" pad="2"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
<device name="-0612" package="SMD0612">
<connects>
<connect gate="G$1" pin="1" pad="1"/>
<connect gate="G$1" pin="2" pad="2"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
<device name="-0805" package="SMD0805">
<connects>
<connect gate="G$1" pin="1" pad="1"/>
<connect gate="G$1" pin="2" pad="2"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
<device name="-1206" package="SMD1206">
<connects>
<connect gate="G$1" pin="1" pad="1"/>
<connect gate="G$1" pin="2" pad="2"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
<device name="-2512" package="SMD2512">
<connects>
<connect gate="G$1" pin="1" pad="1"/>
<connect gate="G$1" pin="2" pad="2"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
</devicesets>
</library>
<library name="ataradov_vreg">
<description>Alex Taradov Library (Voltage Regulators)</description>
<packages>
<package name="SOT23-5">
<description>SOT23-5</description>
<wire x1="1.422" y1="0.81" x2="1.422" y2="-0.81" width="0.1524" layer="21"/>
<wire x1="1.422" y1="-0.81" x2="-1.422" y2="-0.81" width="0.1524" layer="51"/>
<wire x1="-1.422" y1="-0.81" x2="-1.422" y2="0.81" width="0.1524" layer="21"/>
<wire x1="-1.422" y1="0.81" x2="1.422" y2="0.81" width="0.1524" layer="51"/>
<wire x1="-0.522" y1="0.81" x2="0.522" y2="0.81" width="0.1524" layer="21"/>
<wire x1="-0.428" y1="-0.81" x2="-0.522" y2="-0.81" width="0.1524" layer="21"/>
<wire x1="0.522" y1="-0.81" x2="0.428" y2="-0.81" width="0.1524" layer="21"/>
<wire x1="-1.328" y1="-0.81" x2="-1.422" y2="-0.81" width="0.1524" layer="21"/>
<wire x1="1.422" y1="-0.81" x2="1.328" y2="-0.81" width="0.1524" layer="21"/>
<wire x1="1.328" y1="0.81" x2="1.422" y2="0.81" width="0.1524" layer="21"/>
<wire x1="-1.422" y1="0.81" x2="-1.328" y2="0.81" width="0.1524" layer="21"/>
<smd name="1" x="-0.95" y="-1.3" dx="0.55" dy="1.2" layer="1"/>
<smd name="2" x="0" y="-1.3" dx="0.55" dy="1.2" layer="1"/>
<smd name="3" x="0.95" y="-1.3" dx="0.55" dy="1.2" layer="1"/>
<smd name="4" x="0.95" y="1.3" dx="0.55" dy="1.2" layer="1"/>
<smd name="5" x="-0.95" y="1.3" dx="0.55" dy="1.2" layer="1"/>
<text x="0" y="2.286" size="1.27" layer="25" font="vector" align="bottom-center">&gt;NAME</text>
</package>
</packages>
<symbols>
<symbol name="VREG-3-EN">
<wire x1="-7.62" y1="5.08" x2="7.62" y2="5.08" width="0.254" layer="94"/>
<wire x1="7.62" y1="5.08" x2="7.62" y2="-2.54" width="0.254" layer="94"/>
<wire x1="7.62" y1="-2.54" x2="-7.62" y2="-2.54" width="0.254" layer="94"/>
<wire x1="-7.62" y1="-2.54" x2="-7.62" y2="5.08" width="0.254" layer="94"/>
<text x="-2.032" y="-1.524" size="1.524" layer="95">GND</text>
<text x="0" y="5.842" size="1.27" layer="95" align="bottom-center">&gt;NAME</text>
<text x="1.524" y="-4.318" size="1.27" layer="96">&gt;VALUE</text>
<pin name="IN" x="-10.16" y="2.54" length="short" direction="pas"/>
<pin name="OUT" x="10.16" y="2.54" length="short" direction="pas" rot="R180"/>
<pin name="GND" x="0" y="-5.08" visible="pad" length="short" direction="pas" rot="R90"/>
<pin name="EN" x="-10.16" y="0" length="short" direction="pas"/>
</symbol>
</symbols>
<devicesets>
<deviceset name="MIC5504" prefix="IC" uservalue="yes">
<description>LM1117 voltage regulator</description>
<gates>
<gate name="G$1" symbol="VREG-3-EN" x="0" y="0"/>
</gates>
<devices>
<device name="-SOT23-5" package="SOT23-5">
<connects>
<connect gate="G$1" pin="EN" pad="3"/>
<connect gate="G$1" pin="GND" pad="2"/>
<connect gate="G$1" pin="IN" pad="1"/>
<connect gate="G$1" pin="OUT" pad="5"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
</devicesets>
</library>
<library name="ataradov_misc">
<description>Alex Taradov Library (Miscellaneous stuff)</description>
<packages>
<package name="TP-1.27MM">
<smd name="1" x="0" y="0" dx="1.27" dy="1.27" layer="1" roundness="100" cream="no"/>
<text x="0" y="1.016" size="1.27" layer="25" font="vector" align="bottom-center">&gt;NAME</text>
</package>
<package name="TP-2.54MM">
<smd name="1" x="0" y="0" dx="2.54" dy="2.54" layer="1" roundness="100" cream="no"/>
<text x="0" y="1.778" size="1.27" layer="25" font="vector" align="bottom-center">&gt;NAME</text>
</package>
<package name="SB-1.27MM-NO">
<smd name="1" x="-0.508" y="0" dx="0.762" dy="1.27" layer="1" cream="no"/>
<text x="0" y="1.016" size="1.27" layer="25" font="vector" align="bottom-center">&gt;NAME</text>
<text x="0" y="-2.286" size="1.27" layer="27" font="vector" align="bottom-center">&gt;VALUE</text>
<smd name="2" x="0.508" y="0" dx="0.762" dy="1.27" layer="1" cream="no"/>
<rectangle x1="-1.016" y1="-0.762" x2="1.016" y2="0.762" layer="29"/>
</package>
<package name="TP-0.635MM">
<smd name="1" x="0" y="0" dx="0.635" dy="0.635" layer="1" roundness="100" cream="no"/>
<text x="0" y="1.016" size="1.27" layer="25" font="vector" align="bottom-center">&gt;NAME</text>
</package>
<package name="TP-0.5MM">
<smd name="1" x="0" y="0" dx="0.5" dy="0.5" layer="1" roundness="100" cream="no"/>
<text x="0" y="1.016" size="1.27" layer="25" font="vector" align="bottom-center">&gt;NAME</text>
</package>
<package name="TP-1.27MM-TH">
<text x="0" y="1.016" size="1.27" layer="25" font="vector" align="bottom-center">&gt;NAME</text>
<pad name="1" x="0" y="0" drill="0.762" diameter="1.27"/>
</package>
</packages>
<symbols>
<symbol name="TP">
<text x="4.064" y="-0.508" size="1.27" layer="95">&gt;NAME</text>
<pin name="1" x="0" y="0" visible="off" length="short" direction="pas"/>
<circle x="3.048" y="0" radius="0.508" width="0" layer="94"/>
</symbol>
<symbol name="SB">
<wire x1="0.381" y1="0.381" x2="0.381" y2="-0.381" width="0.762" layer="94" curve="-180" cap="flat"/>
<wire x1="-0.381" y1="-0.381" x2="-0.381" y2="0.381" width="0.762" layer="94" curve="-180" cap="flat"/>
<wire x1="2.54" y1="0" x2="1.143" y2="0" width="0.1524" layer="94"/>
<wire x1="-2.54" y1="0" x2="-1.143" y2="0" width="0.1524" layer="94"/>
<text x="0" y="1.778" size="1.27" layer="95" align="bottom-center">&gt;NAME</text>
<pin name="2" x="2.54" y="0" visible="off" length="point" direction="pas" swaplevel="1" rot="R180"/>
<pin name="1" x="-2.54" y="0" visible="off" length="point" direction="pas" swaplevel="1"/>
<text x="0" y="-3.048" size="1.27" layer="96" align="bottom-center">&gt;VALUE</text>
</symbol>
</symbols>
<devicesets>
<deviceset name="TEST-POINT" prefix="TP" uservalue="yes">
<description>Test Point</description>
<gates>
<gate name="G$1" symbol="TP" x="0" y="0"/>
</gates>
<devices>
<device name="-1.27MM" package="TP-1.27MM">
<connects>
<connect gate="G$1" pin="1" pad="1"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
<device name="-0.635MM" package="TP-0.635MM">
<connects>
<connect gate="G$1" pin="1" pad="1"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
<device name="-0.5MM" package="TP-0.5MM">
<connects>
<connect gate="G$1" pin="1" pad="1"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
<device name="-2.54MM" package="TP-2.54MM">
<connects>
<connect gate="G$1" pin="1" pad="1"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
<device name="-1.27MM-TH" package="TP-1.27MM-TH">
<connects>
<connect gate="G$1" pin="1" pad="1"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
<deviceset name="SOLDER-BRIDGE" prefix="SB" uservalue="yes">
<description>Solder Bridge</description>
<gates>
<gate name="G$1" symbol="SB" x="0" y="0"/>
</gates>
<devices>
<device name="-1.27MM-NO" package="SB-1.27MM-NO">
<connects>
<connect gate="G$1" pin="1" pad="1"/>
<connect gate="G$1" pin="2" pad="2"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
</devicesets>
</library>
<library name="ataradov_led">
<description>Alex Taradov Library (LEDs and other indication devices)</description>
<packages>
<package name="SMD0603">
<wire x1="-1.5" y1="0.75" x2="1.5" y2="0.75" width="0.1" layer="21"/>
<wire x1="1.5" y1="0.75" x2="1.5" y2="-0.75" width="0.1" layer="21"/>
<wire x1="1.5" y1="-0.75" x2="-1.5" y2="-0.75" width="0.1" layer="21"/>
<wire x1="-1.5" y1="-0.75" x2="-1.5" y2="0.75" width="0.1" layer="21"/>
<smd name="1" x="-0.8" y="0" dx="0.9" dy="0.9" layer="1"/>
<smd name="2" x="0.8" y="0" dx="0.9" dy="0.9" layer="1"/>
<text x="0" y="1.016" size="1.27" layer="25" font="vector" align="bottom-center">&gt;NAME</text>
<text x="0" y="-0.254" size="0.508" layer="27" font="vector" align="bottom-center">&gt;VALUE</text>
</package>
<package name="SMD0805">
<wire x1="-2" y1="1" x2="2" y2="1" width="0.1" layer="21"/>
<wire x1="2" y1="1" x2="2" y2="-1" width="0.1" layer="21"/>
<wire x1="2" y1="-1" x2="-2" y2="-1" width="0.1" layer="21"/>
<wire x1="-2" y1="-1" x2="-2" y2="1" width="0.1" layer="21"/>
<smd name="1" x="-0.95" y="0" dx="1.3" dy="1.5" layer="1"/>
<smd name="2" x="0.95" y="0" dx="1.3" dy="1.5" layer="1"/>
<text x="0" y="1.27" size="1.27" layer="25" font="vector" align="bottom-center">&gt;NAME</text>
<text x="0" y="-0.254" size="0.508" layer="27" font="vector" align="bottom-center">&gt;VALUE</text>
</package>
</packages>
<symbols>
<symbol name="LED">
<wire x1="1.27" y1="0" x2="0" y2="-2.54" width="0.254" layer="94"/>
<wire x1="0" y1="-2.54" x2="-1.27" y2="0" width="0.254" layer="94"/>
<wire x1="1.27" y1="-2.54" x2="0" y2="-2.54" width="0.254" layer="94"/>
<wire x1="0" y1="-2.54" x2="-1.27" y2="-2.54" width="0.254" layer="94"/>
<wire x1="1.27" y1="0" x2="0" y2="0" width="0.254" layer="94"/>
<wire x1="0" y1="0" x2="-1.27" y2="0" width="0.254" layer="94"/>
<wire x1="-2.032" y1="-0.762" x2="-3.429" y2="-2.159" width="0.1524" layer="94"/>
<wire x1="-1.905" y1="-1.905" x2="-3.302" y2="-3.302" width="0.1524" layer="94"/>
<text x="3.556" y="-2.032" size="1.27" layer="95" rot="R90" align="bottom-center">&gt;NAME</text>
<text x="5.715" y="-2.032" size="1.27" layer="96" rot="R90" align="bottom-center">&gt;VALUE</text>
<pin name="C" x="0" y="-5.08" visible="off" length="short" direction="pas" rot="R90"/>
<pin name="A" x="0" y="2.54" visible="off" length="short" direction="pas" rot="R270"/>
<polygon width="0.1524" layer="94">
<vertex x="-3.429" y="-2.159"/>
<vertex x="-3.048" y="-1.27"/>
<vertex x="-2.54" y="-1.778"/>
</polygon>
<polygon width="0.1524" layer="94">
<vertex x="-3.302" y="-3.302"/>
<vertex x="-2.921" y="-2.413"/>
<vertex x="-2.413" y="-2.921"/>
</polygon>
</symbol>
</symbols>
<devicesets>
<deviceset name="LED_SMD" prefix="LED" uservalue="yes">
<description>SMD_LED</description>
<gates>
<gate name="G$1" symbol="LED" x="0" y="0"/>
</gates>
<devices>
<device name="LED_0603" package="SMD0603">
<connects>
<connect gate="G$1" pin="A" pad="1"/>
<connect gate="G$1" pin="C" pad="2"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
<device name="LED_0805" package="SMD0805">
<connects>
<connect gate="G$1" pin="A" pad="1"/>
<connect gate="G$1" pin="C" pad="2"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
</devicesets>
</library>
</libraries>
<attributes>
</attributes>
<variantdefs>
</variantdefs>
<classes>
<class number="0" name="default" width="0" drill="0">
</class>
</classes>
<parts>
<part name="P_8" library="ataradov_pwr" deviceset="+3V3" device=""/>
<part name="P_2" library="ataradov_pwr" deviceset="GND" device=""/>
<part name="P_1" library="ataradov_pwr" deviceset="V_USB" device=""/>
<part name="P_3" library="ataradov_pwr" deviceset="V_USB" device=""/>
<part name="C1" library="ataradov_rlc" deviceset="C" device="-0603" value="1uF"/>
<part name="C2" library="ataradov_rlc" deviceset="C" device="-0603" value="1uF"/>
<part name="IC2" library="ataradov_vreg" deviceset="MIC5504" device="-SOT23-5" value="MIC5504-3.3"/>
<part name="P_5" library="ataradov_pwr" deviceset="GND" device=""/>
<part name="P_9" library="ataradov_pwr" deviceset="GND" device=""/>
<part name="P_4" library="ataradov_pwr" deviceset="GND" device=""/>
<part name="TP1" library="ataradov_misc" deviceset="TEST-POINT" device="-1.27MM" value="SIO"/>
<part name="TP2" library="ataradov_misc" deviceset="TEST-POINT" device="-1.27MM"/>
<part name="TP3" library="ataradov_misc" deviceset="TEST-POINT" device="-1.27MM"/>
<part name="TP4" library="ataradov_misc" deviceset="TEST-POINT" device="-1.27MM"/>
<part name="J2" library="ataradov_conn" deviceset="HEADER-5X2" device="-TH-1.27"/>
<part name="P_11" library="ataradov_pwr" deviceset="GND" device=""/>
<part name="J1" library="ataradov_conn" deviceset="USB-B-MICRO" device="-SMT"/>
<part name="R1" library="ataradov_rlc" deviceset="R" device="-0603" value="10K"/>
<part name="LED1" library="ataradov_led" deviceset="LED_SMD" device="LED_0603" value="ORANGE"/>
<part name="R2" library="ataradov_rlc" deviceset="R" device="-0603" value="10K"/>
<part name="LED2" library="ataradov_led" deviceset="LED_SMD" device="LED_0603" value="GREEN"/>
<part name="P_12" library="ataradov_pwr" deviceset="GND" device=""/>
<part name="J3" library="ataradov_conn" deviceset="CONN-SINGLE" device="-TH" value="RX"/>
<part name="J4" library="ataradov_conn" deviceset="CONN-SINGLE" device="-TH" value="TX"/>
<part name="J5" library="ataradov_conn" deviceset="CONN-SINGLE" device="-TH" value="GND"/>
<part name="P_13" library="ataradov_pwr" deviceset="GND" device=""/>
<part name="SB1" library="ataradov_misc" deviceset="SOLDER-BRIDGE" device="-1.27MM-NO"/>
<part name="IC1" library="ataradov_mcu" deviceset="ATSAMD11D" device=""/>
<part name="P_6" library="ataradov_pwr" deviceset="+3V3" device=""/>
<part name="P_7" library="ataradov_pwr" deviceset="GND" device=""/>
<part name="P_10" library="ataradov_pwr" deviceset="+3V3" device=""/>
</parts>
<sheets>
<sheet>
<plain>
<frame x1="0" y1="0" x2="139.7" y2="114.3" columns="8" rows="5" layer="97"/>
<text x="35.56" y="7.62" size="1.778" layer="97">d11_micro_std_vcp_v3
Copyright (c) 2021, Alex Taradov &lt;alex@taradov.com&gt;
MCU firmware source code is available at https://github.com/ataradov/free-dap</text>
</plain>
<instances>
<instance part="P_8" gate="1" x="76.2" y="35.56"/>
<instance part="P_2" gate="1" x="30.48" y="17.78" rot="MR0"/>
<instance part="P_1" gate="1" x="30.48" y="35.56" rot="MR0"/>
<instance part="P_3" gate="1" x="48.26" y="35.56"/>
<instance part="C1" gate="G$1" x="48.26" y="27.94" rot="R90"/>
<instance part="C2" gate="G$1" x="76.2" y="27.94" rot="R90"/>
<instance part="IC2" gate="G$1" x="60.96" y="30.48"/>
<instance part="P_5" gate="1" x="60.96" y="22.86"/>
<instance part="P_9" gate="1" x="76.2" y="22.86"/>
<instance part="P_4" gate="1" x="48.26" y="22.86"/>
<instance part="TP1" gate="G$1" x="63.5" y="66.04"/>
<instance part="TP2" gate="G$1" x="63.5" y="68.58"/>
<instance part="TP3" gate="G$1" x="63.5" y="71.12"/>
<instance part="TP4" gate="G$1" x="63.5" y="63.5"/>
<instance part="J2" gate="G$1" x="99.06" y="81.28"/>
<instance part="P_11" gate="1" x="88.9" y="71.12"/>
<instance part="J1" gate="G$1" x="20.32" y="27.94" rot="MR0"/>
<instance part="R1" gate="G$1" x="109.22" y="35.56"/>
<instance part="LED1" gate="G$1" x="99.06" y="35.56" rot="MR270"/>
<instance part="R2" gate="G$1" x="109.22" y="25.4"/>
<instance part="LED2" gate="G$1" x="99.06" y="25.4" rot="MR270"/>
<instance part="P_12" gate="1" x="114.3" y="22.86"/>
<instance part="J3" gate="G$1" x="104.14" y="53.34"/>
<instance part="J4" gate="G$1" x="104.14" y="50.8"/>
<instance part="J5" gate="G$1" x="104.14" y="48.26"/>
<instance part="P_13" gate="1" x="99.06" y="45.72"/>
<instance part="SB1" gate="G$1" x="88.9" y="88.9" rot="R270"/>
<instance part="IC1" gate="G$1" x="43.18" y="76.2"/>
<instance part="P_6" gate="1" x="60.96" y="93.98"/>
<instance part="P_7" gate="1" x="60.96" y="58.42"/>
<instance part="P_10" gate="1" x="88.9" y="93.98"/>
</instances>
<busses>
</busses>
<nets>
<net name="USB_DM" class="0">
<segment>
<wire x1="30.48" y1="30.48" x2="27.94" y2="30.48" width="0.1524" layer="91"/>
<pinref part="J1" gate="G$1" pin="DM"/>
<label x="30.48" y="30.48" size="1.27" layer="95"/>
</segment>
<segment>
<pinref part="IC1" gate="G$1" pin="PA24/DM"/>
<wire x1="60.96" y1="78.74" x2="63.5" y2="78.74" width="0.1524" layer="91"/>
<label x="63.5" y="78.74" size="1.27" layer="95"/>
</segment>
</net>
<net name="USB_DP" class="0">
<segment>
<wire x1="30.48" y1="27.94" x2="27.94" y2="27.94" width="0.1524" layer="91"/>
<pinref part="J1" gate="G$1" pin="DP"/>
<label x="30.48" y="27.94" size="1.27" layer="95"/>
</segment>
<segment>
<pinref part="IC1" gate="G$1" pin="PA25/DP"/>
<wire x1="60.96" y1="76.2" x2="63.5" y2="76.2" width="0.1524" layer="91"/>
<label x="63.5" y="76.2" size="1.27" layer="95"/>
</segment>
</net>
<net name="GND" class="0">
<segment>
<pinref part="P_2" gate="1" pin="GND"/>
<wire x1="30.48" y1="20.32" x2="30.48" y2="22.86" width="0.1524" layer="91"/>
<wire x1="30.48" y1="22.86" x2="27.94" y2="22.86" width="0.1524" layer="91"/>
<pinref part="J1" gate="G$1" pin="GND"/>
</segment>
<segment>
<pinref part="IC2" gate="G$1" pin="GND"/>
<pinref part="P_5" gate="1" pin="GND"/>
</segment>
<segment>
<pinref part="C1" gate="G$1" pin="1"/>
<pinref part="P_4" gate="1" pin="GND"/>
</segment>
<segment>
<pinref part="C2" gate="G$1" pin="1"/>
<pinref part="P_9" gate="1" pin="GND"/>
</segment>
<segment>
<pinref part="TP4" gate="G$1" pin="1"/>
<pinref part="IC1" gate="G$1" pin="PAD"/>
<pinref part="P_7" gate="1" pin="GND"/>
<pinref part="IC1" gate="G$1" pin="GND"/>
<wire x1="60.96" y1="60.96" x2="60.96" y2="63.5" width="0.1524" layer="91"/>
<junction x="60.96" y="60.96"/>
<wire x1="63.5" y1="63.5" x2="60.96" y2="63.5" width="0.1524" layer="91"/>
<junction x="60.96" y="63.5"/>
</segment>
<segment>
<pinref part="J2" gate="G$1" pin="3"/>
<wire x1="91.44" y1="83.82" x2="88.9" y2="83.82" width="0.1524" layer="91"/>
<pinref part="P_11" gate="1" pin="GND"/>
<wire x1="88.9" y1="83.82" x2="88.9" y2="81.28" width="0.1524" layer="91"/>
<pinref part="J2" gate="G$1" pin="5"/>
<wire x1="88.9" y1="81.28" x2="88.9" y2="76.2" width="0.1524" layer="91"/>
<wire x1="88.9" y1="76.2" x2="88.9" y2="73.66" width="0.1524" layer="91"/>
<wire x1="91.44" y1="81.28" x2="88.9" y2="81.28" width="0.1524" layer="91"/>
<junction x="88.9" y="81.28"/>
<pinref part="J2" gate="G$1" pin="9"/>
<wire x1="91.44" y1="76.2" x2="88.9" y2="76.2" width="0.1524" layer="91"/>
<junction x="88.9" y="76.2"/>
</segment>
<segment>
<pinref part="P_12" gate="1" pin="GND"/>
<wire x1="114.3" y1="35.56" x2="114.3" y2="25.4" width="0.1524" layer="91"/>
<pinref part="R1" gate="G$1" pin="2"/>
<pinref part="R2" gate="G$1" pin="2"/>
<junction x="114.3" y="25.4"/>
</segment>
<segment>
<pinref part="J5" gate="G$1" pin="1"/>
<pinref part="P_13" gate="1" pin="GND"/>
<wire x1="99.06" y1="48.26" x2="101.6" y2="48.26" width="0.1524" layer="91"/>
</segment>
</net>
<net name="V_USB" class="0">
<segment>
<pinref part="P_1" gate="1" pin="V_USB"/>
<wire x1="30.48" y1="33.02" x2="27.94" y2="33.02" width="0.1524" layer="91"/>
<pinref part="J1" gate="G$1" pin="VBUS"/>
</segment>
<segment>
<pinref part="IC2" gate="G$1" pin="EN"/>
<pinref part="P_3" gate="1" pin="V_USB"/>
<wire x1="50.8" y1="30.48" x2="48.26" y2="30.48" width="0.1524" layer="91"/>
<pinref part="IC2" gate="G$1" pin="IN"/>
<wire x1="50.8" y1="33.02" x2="48.26" y2="33.02" width="0.1524" layer="91"/>
<junction x="48.26" y="33.02"/>
<pinref part="C1" gate="G$1" pin="2"/>
<wire x1="48.26" y1="33.02" x2="48.26" y2="30.48" width="0.1524" layer="91"/>
<junction x="48.26" y="30.48"/>
</segment>
</net>
<net name="+3V3" class="0">
<segment>
<pinref part="IC2" gate="G$1" pin="OUT"/>
<wire x1="71.12" y1="33.02" x2="76.2" y2="33.02" width="0.1524" layer="91"/>
<pinref part="C2" gate="G$1" pin="2"/>
<wire x1="76.2" y1="30.48" x2="76.2" y2="33.02" width="0.1524" layer="91"/>
<junction x="76.2" y="33.02"/>
<pinref part="P_8" gate="1" pin="+3V3"/>
</segment>
<segment>
<pinref part="IC1" gate="G$1" pin="VDD"/>
<pinref part="P_6" gate="1" pin="+3V3"/>
</segment>
<segment>
<pinref part="SB1" gate="G$1" pin="1"/>
<pinref part="P_10" gate="1" pin="+3V3"/>
</segment>
</net>
<net name="SWDIO" class="0">
<segment>
<pinref part="IC1" gate="G$1" pin="PA31/SIO"/>
<pinref part="TP1" gate="G$1" pin="1"/>
<wire x1="60.96" y1="66.04" x2="63.5" y2="66.04" width="0.1524" layer="91"/>
</segment>
</net>
<net name="SWCLK" class="0">
<segment>
<pinref part="IC1" gate="G$1" pin="PA30/SCK"/>
<pinref part="TP2" gate="G$1" pin="1"/>
<wire x1="60.96" y1="68.58" x2="63.5" y2="68.58" width="0.1524" layer="91"/>
</segment>
</net>
<net name="RESET" class="0">
<segment>
<pinref part="TP3" gate="G$1" pin="1"/>
<pinref part="IC1" gate="G$1" pin="PA28/RST"/>
<wire x1="63.5" y1="71.12" x2="60.96" y2="71.12" width="0.1524" layer="91"/>
</segment>
</net>
<net name="T_SWDIO_TMS" class="0">
<segment>
<pinref part="J2" gate="G$1" pin="2"/>
<wire x1="106.68" y1="86.36" x2="109.22" y2="86.36" width="0.1524" layer="91"/>
<label x="109.22" y="86.36" size="1.27" layer="95"/>
</segment>
<segment>
<pinref part="IC1" gate="G$1" pin="PA08"/>
<wire x1="25.4" y1="76.2" x2="22.86" y2="76.2" width="0.1524" layer="91"/>
<label x="22.86" y="76.2" size="1.27" layer="95" rot="MR0"/>
</segment>
</net>
<net name="T_SWCLK_TCK" class="0">
<segment>
<pinref part="J2" gate="G$1" pin="4"/>
<wire x1="109.22" y1="83.82" x2="106.68" y2="83.82" width="0.1524" layer="91"/>
<label x="109.22" y="83.82" size="1.27" layer="95"/>
</segment>
<segment>
<pinref part="IC1" gate="G$1" pin="PA09"/>
<wire x1="25.4" y1="73.66" x2="22.86" y2="73.66" width="0.1524" layer="91"/>
<label x="22.86" y="73.66" size="1.27" layer="95" rot="MR0"/>
</segment>
</net>
<net name="T_RESET" class="0">
<segment>
<pinref part="J2" gate="G$1" pin="10"/>
<wire x1="106.68" y1="76.2" x2="109.22" y2="76.2" width="0.1524" layer="91"/>
<label x="109.22" y="76.2" size="1.27" layer="95"/>
</segment>
<segment>
<pinref part="IC1" gate="G$1" pin="PA15"/>
<wire x1="25.4" y1="63.5" x2="22.86" y2="63.5" width="0.1524" layer="91"/>
<label x="22.86" y="63.5" size="1.27" layer="95" rot="MR0"/>
</segment>
</net>
<net name="UART_TX" class="0">
<segment>
<pinref part="J4" gate="G$1" pin="1"/>
<wire x1="101.6" y1="50.8" x2="96.52" y2="50.8" width="0.1524" layer="91"/>
<label x="96.52" y="50.8" size="1.27" layer="95" rot="MR0"/>
</segment>
<segment>
<pinref part="IC1" gate="G$1" pin="PA16"/>
<wire x1="25.4" y1="60.96" x2="22.86" y2="60.96" width="0.1524" layer="91"/>
<label x="22.86" y="60.96" size="1.27" layer="95" rot="MR0"/>
</segment>
</net>
<net name="UART_RX" class="0">
<segment>
<pinref part="J3" gate="G$1" pin="1"/>
<wire x1="101.6" y1="53.34" x2="96.52" y2="53.34" width="0.1524" layer="91"/>
<label x="96.52" y="53.34" size="1.27" layer="95" rot="MR0"/>
</segment>
<segment>
<pinref part="IC1" gate="G$1" pin="PA17"/>
<wire x1="60.96" y1="88.9" x2="63.5" y2="88.9" width="0.1524" layer="91"/>
<label x="63.5" y="88.9" size="1.27" layer="95"/>
</segment>
</net>
<net name="T_TDO" class="0">
<segment>
<pinref part="J2" gate="G$1" pin="6"/>
<wire x1="106.68" y1="81.28" x2="109.22" y2="81.28" width="0.1524" layer="91"/>
<label x="109.22" y="81.28" size="1.27" layer="95"/>
</segment>
<segment>
<pinref part="IC1" gate="G$1" pin="PA10"/>
<wire x1="25.4" y1="71.12" x2="22.86" y2="71.12" width="0.1524" layer="91"/>
<label x="22.86" y="71.12" size="1.27" layer="95" rot="MR0"/>
</segment>
</net>
<net name="T_TDI" class="0">
<segment>
<pinref part="J2" gate="G$1" pin="8"/>
<wire x1="106.68" y1="78.74" x2="109.22" y2="78.74" width="0.1524" layer="91"/>
<label x="109.22" y="78.74" size="1.27" layer="95"/>
</segment>
<segment>
<pinref part="IC1" gate="G$1" pin="PA14"/>
<wire x1="25.4" y1="66.04" x2="22.86" y2="66.04" width="0.1524" layer="91"/>
<label x="22.86" y="66.04" size="1.27" layer="95" rot="MR0"/>
</segment>
</net>
<net name="N$2" class="0">
<segment>
<pinref part="R1" gate="G$1" pin="1"/>
<pinref part="LED1" gate="G$1" pin="C"/>
</segment>
</net>
<net name="N$3" class="0">
<segment>
<pinref part="R2" gate="G$1" pin="1"/>
<pinref part="LED2" gate="G$1" pin="C"/>
</segment>
</net>
<net name="LED_A" class="0">
<segment>
<pinref part="LED1" gate="G$1" pin="A"/>
<wire x1="96.52" y1="35.56" x2="93.98" y2="35.56" width="0.1524" layer="91"/>
<label x="93.98" y="35.56" size="1.27" layer="95" rot="MR0"/>
</segment>
<segment>
<pinref part="IC1" gate="G$1" pin="PA03"/>
<wire x1="25.4" y1="88.9" x2="22.86" y2="88.9" width="0.1524" layer="91"/>
<label x="22.86" y="88.9" size="1.27" layer="95" rot="MR0"/>
</segment>
</net>
<net name="LED_B" class="0">
<segment>
<pinref part="LED2" gate="G$1" pin="A"/>
<wire x1="96.52" y1="25.4" x2="93.98" y2="25.4" width="0.1524" layer="91"/>
<label x="93.98" y="25.4" size="1.27" layer="95" rot="MR0"/>
</segment>
<segment>
<pinref part="IC1" gate="G$1" pin="PA06"/>
<wire x1="25.4" y1="81.28" x2="22.86" y2="81.28" width="0.1524" layer="91"/>
<label x="22.86" y="81.28" size="1.27" layer="95" rot="MR0"/>
</segment>
</net>
<net name="T_VREF" class="0">
<segment>
<pinref part="J2" gate="G$1" pin="1"/>
<pinref part="SB1" gate="G$1" pin="2"/>
<wire x1="88.9" y1="86.36" x2="91.44" y2="86.36" width="0.1524" layer="91"/>
</segment>
</net>
</nets>
</sheet>
</sheets>
</schematic>
</drawing>
<compatibility>
<note version="6.3" minversion="6.2.2" severity="warning">
Since Version 6.2.2 text objects can contain more than one line,
which will not be processed correctly with this version.
</note>
</compatibility>
</eagle>
