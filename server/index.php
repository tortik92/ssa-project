<?php

header('Access-Control-Allow-Origin: *');
header('Access-Control-Allow-Methods: *');
header('Access-Control-Allow-Headers: *');
header('Access-Control-Max-Age: 1728000');
header('Content-Type: application/json');

$uid = $_GET['uid'] ?? null;
$jsonData = json_decode(file_get_contents("games.json"), true);

if ($jsonData === null) {
    http_response_code(500);
    echo json_encode(["error" => "Failed to decode JSON data."]);
    exit;
}
$responseData = [];

// Function to filter out "preferences" from each data entry
function filterPreferences(array $data): array {
    foreach ($data as &$entry) {
        unset($entry['preferences']);
    }
    return $data;
}

// Find matching data based on `uid`, or return filtered data if no uid is specified
if ($uid) {
    $matchingUser = array_filter($jsonData, fn($item) => $item['uid'] === $uid);
    $responseData = array_values($matchingUser); // Reset keys for JSON encoding consistency
} else {
    $responseData = filterPreferences($jsonData);
}

echo json_encode($responseData);
exit;
