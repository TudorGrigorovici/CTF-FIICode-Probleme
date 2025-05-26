<?php
require '../vendor/autoload.php';
use Firebase\JWT\JWT;
use Firebase\JWT\Key;

$secret_key = "ASIIRULS";

function decode_jwt($jwt, $secret_key) {
    try {
        $decoded = JWT::decode($jwt, new Key($secret_key, 'HS256'));
        return (array)$decoded;
    } catch (Exception $e) {
        return null;
    }
}

$jwt = $_COOKIE['token'] ?? null;
$payload = $jwt ? decode_jwt($jwt, $secret_key) : null;

if (!$payload || !$payload['artist']) {
    http_response_code(403);
    die("Access denied: only artists can upload.");
}

if (isset($_POST['upload']) && isset($_FILES['song_file'])) {
    $upload_dir = "/var/www/html/resources/songs/";
    if (!is_dir($upload_dir)) {
        mkdir($upload_dir, 0755, true);
    }

    $file = $_FILES['song_file'];
    $filename = basename($file['name']);
    $target_file = $upload_dir . $filename;

    $ext = strtolower($filename);
    if (strpos($ext, 'mp3') === false) {
        die("Only MP3 files are allowed!");
    }

    $finfo = finfo_open(FILEINFO_MIME_TYPE);
    $mime = finfo_file($finfo, $file['tmp_name']);
    finfo_close($finfo);
    if ($mime !== 'audio/mpeg') {
        die("Invalid file type.".$mime." ".$ext);
    }

    if (move_uploaded_file($file['tmp_name'], $target_file)) {
        echo "File uploaded successfully!";
    } else {
        echo "Upload failed.";
    }
} else {
    echo "No file uploaded.";
}

echo "<h2>PHP Source Code</h2>";
highlight_string("<?php\n" . file_get_contents(__FILE__) . "\n?>");
?>
