<?php
require 'vendor/autoload.php';
use Firebase\JWT\JWT;
use Firebase\JWT\Key;

// Set your secret key
$secret_key = "ASIIRULS";

// Function to decode the JWT and extract the payload
function decode_jwt($jwt, $secret_key) {
    try {
        $decoded = JWT::decode($jwt, new Key($secret_key, 'HS256'));
        return (array) $decoded;
    } catch (Exception $e) {
        return null;
    }
}

// Get the JWT from the cookie
$jwt = isset($_COOKIE['token']) ? $_COOKIE['token'] : null;
$payload = null;

// Decode the JWT if it exists
if ($jwt) {
    $payload = decode_jwt($jwt, $secret_key);
}

// Flag to check if the user is an artist
$is_artist = $payload && isset($payload['artist']) && $payload['artist'] == true;

// Directory containing songs
$song_directory = 'resources/songs/';

// Get a list of MP3 files from the directory
$songs = array_filter(scandir($song_directory), function($file) use ($song_directory) {
    return pathinfo($file, PATHINFO_EXTENSION) == 'mp3' && is_file($song_directory . $file);
});

?>

<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>VibeX</title>
</head>
<body style="font-family: Arial, sans-serif; background-color: #111; color: #fff; text-align: center; padding-top: 60px;">

    <nav style="width: 100%; background-color: #222; padding: 10px 20px; position: fixed; top: 0; left: 0; display: flex; justify-content: space-between; align-items: center;">
        <div style="color: #0f0; font-weight: bold;">VibeX</div>
        <div style="flex-grow: 1; display: flex; justify-content: center;">
            <a href="index.php" style="color: white; text-decoration: none; margin-right: 35px;">Home</a>
            <a href="profile.php" style="color: white; text-decoration: none;">Profile</a>
        </div>
    </nav>

    <h1>Welcome to VibeX!</h1>

    <?php if ($is_artist): ?>
        <h2>You are an artist. Upload your song:</h2>

        <!-- Upload Form -->
        <form action="dev/upload.php" method="POST" enctype="multipart/form-data">
            <label for="song_file">Choose MP3 File:</label><br>
            <input type="file" name="song_file" accept=".mp3" required><br><br>
            <button type="submit" name="upload">Upload</button>
        </form>
    <?php else: ?>
        <h2>Listen to a song:</h2>

        <!-- Display MP3 Files from the directory -->
        <?php if (!empty($songs)): ?>
            <ul style="list-style-type: none; padding: 0;">
                <?php foreach ($songs as $song): ?>
                    <li style="margin-bottom: 20px;">
                        <h3><?php echo htmlspecialchars($song); ?></h3>
                        <audio controls>
                            <source src="resources/songs/<?php echo htmlspecialchars($song); ?>" type="audio/mp3">
                            Your browser does not support the audio element.
                        </audio>
                    </li>
                <?php endforeach; ?>
            </ul>
        <?php else: ?>
            <p>No songs available to play.</p>
        <?php endif; ?>
    <?php endif; ?>

</body>
</html>
