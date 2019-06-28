<?php
   if(!defined('INDEX')) die("");

   $halaman = array("dashboard", 
      "kontrol", "informasi", "aboutus");

   if(isset($_GET['hal'])) $hal = $_GET['hal'];
   else $hal = "dashboard";

   foreach($halaman as $h){
      if($hal == $h){
         include "content/$h.php";
         break;
      }
   }
?>