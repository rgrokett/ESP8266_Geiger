<?php

	header("Content-Type: text/plain");

	$file = "geiger.log";

	$cpm = htmlspecialchars($_GET["cpm"]);
	$usvh = htmlspecialchars($_GET["usvh"]);

	$datetime = date("Y-m-dTH:i:s");

	$data = "dt=$datetime|cpm=$cpm|usvh=$usvh\n";
	file_put_contents($file, $data, FILE_APPEND );

	# SEND TO https://thingspeak.com/channels/77779
	$URL = "https://api.thingspeak.com/update?api_key=BFXZG7F92XMNVIP4&field1=$datetime&field2=$cpm&field3=$usvh";
	$resp = file_get_contents("$URL");

	echo "OK $datetime\n";

?>
