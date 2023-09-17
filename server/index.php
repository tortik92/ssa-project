<?php
    header('Content-Type: application/json; charset=utf-8');
    header("Access-Control-Allow-Origin: *");

    $uid = "";
    if (isset($_GET['uid'])) {
        $uid = $_GET['uid'];
    }

    $json = file_get_contents("games.json");
    $json_data = json_decode($json, true);

    $output = array();
    $any = 0;
    if($uid != "") {
        for($i = 0; $i < sizeof($json_data); $i++) {
            if($json_data[$i]["uid"] == $uid) {
                $output = $json_data[$i];
                $any++;
            }
        }
    }
    else {
        $output = $json_data;
    }

    echo json_encode($output);
?>


