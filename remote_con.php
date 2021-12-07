<?php
    if(isset($_GET['LEDON'])){
       $value = shell_exec("/home/pi/PHP_LEDON");
       echo $value;
    }else if(isset($_GET['LEDOFF'])){
       $value = shell_exec("/home/pi/PHP_LEDOFF");
       echo $value;
    }else if(isset($_GET['DCMON'])){
                 $value = shell_exec("/home/pi/PHP_DCMON");
                 echo $value;
   }else if(isset($_GET['DCMOFF'])){
                 $value = shell_exec("/home/pi/PHP_DCMOFF");
                 echo $value;
    }else if(isset($_GET['LIGHT'])){
                 $value = shell_exec("/home/pi/PHP_LIGHT");
                 echo $value;
    }else if(isset($_GET['TEMP'])){
                 $value = shell_exec("/home/pi/PHP_TEMP");
                 echo $value;
    }else if(isset($_GET['HUMI'])){
                 $value = shell_exec("/home/pi/PHP_HUMI");
                 echo $value;
    }else if(isset($_GET['DUST'])){
                 $value = shell_exec("/home/pi/PHP_DUST");
                 echo $value;
    }
?>



