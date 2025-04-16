<?php

header('Access-Control-Allow-Origin: *');
header('Access-Control-Allow-Methods: *');
header('Access-Control-Allow-Headers: *');
header('Access-Control-Max-Age: 1728000');

// Get the UID from the URL
$uid = basename($_GET["uid"]);

// Define the path to the games directory
$gamesDir = __DIR__ . '/games/';
$filePath = $uid . ".txt";
// Check if the file exists
if (file_exists($filePath)) {
    // Output or do something with the file
    // Example: read the content
    echo file_get_contents($filePath);
} else {
    // File not found
    http_response_code(404);
    echo "File not found.";
}
?>
