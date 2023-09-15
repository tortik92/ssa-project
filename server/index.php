<?php
    header('Content-Type: application/json; charset=utf-8');

    $uid = "";
    if (isset($_GET['uid'])) {
        $uid = $_GET['uid'];
    }

    $json = file_get_contents("games.json");
    $json_data = json_decode($json, true);

    if($uid != "") {
        for($i = 0; $i < sizeof($json_data); $i++) {
            if($json_data[$i]["uid"] == $uid) {
                echo json_encode($json_data[$i]);
            }
        }
        echo json_encode(array());
    }
    else {
        $outputJSON = json_encode($json_data);
        echo $outputJSON;
    }



?>


