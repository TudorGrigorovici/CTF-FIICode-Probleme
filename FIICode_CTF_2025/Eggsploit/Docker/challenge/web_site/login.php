<?php
ini_set('display_errors', 1);
ini_set('display_startup_errors', 1);
error_reporting(E_ALL);

require 'vendor/autoload.php';

use \Firebase\JWT\JWT;

$db = new SQLite3('/var/www/html/chickenmafia.db');

$message = "";
$jwt = "";

$secretKey = "your_secret_key"; // Secret key used to encode and decode the JWT
$issuedAt = time();
$expirationTime = $issuedAt + 3600;  // jwt valid for 1 hour from the issued time
$issuer = "ChickenMafia";  // Issuer of the JWT

if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    $username = $_POST['username'] ?? '';
    $password = $_POST['password'] ?? '';

    if (!empty($username) && !empty($password)) {
        // Prepare statement to check if the user exists
        $stmt = $db->prepare("SELECT * FROM users WHERE username = :username AND password = :password");
        $stmt->bindValue(':username', $username, SQLITE3_TEXT);
        $stmt->bindValue(':password', $password, SQLITE3_TEXT);
        $result = $stmt->execute();

        if ($result && $result->fetchArray()) {
            // If credentials are correct, create a JWT token
            $payload = array(
                "iss" => $issuer,
                "iat" => $issuedAt,
                "exp" => $expirationTime,
                "username" => $username
            );

            // Encode the payload and generate the token
            $jwt = JWT::encode($payload, $secretKey,'HS256');

                setcookie("token", $jwt, [
        'expires' => $expirationTime,
        'path' => '/',
        'secure' => false,      // Set to true if using HTTPS
        'httponly' => true,    // Prevent JavaScript access
        'samesite' => 'Strict' // Prevent CSRF in most cases
    ]);
            $message = "Login successful! Welcome, $username.";
        } else {
            // Invalid credentials
            $message = "Invalid credentials.";
        }
    } else {
        $message = "Please enter both fields.";
    }
}
?>

<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>Login - ChickenMafia</title>
    <link href="https://fonts.googleapis.com/css2?family=Poppins:wght@400;600&display=swap" rel="stylesheet">
    <style>
        body {
            background-color: #fafafa;
            font-family: 'Poppins', sans-serif;
            display: flex;
            align-items: center;
            justify-content: center;
            min-height: 100vh;
            margin: 0;
        }

        .login-container {
            background-color: #fff;
            padding: 40px 50px;
            border-radius: 12px;
            box-shadow: 0 10px 30px rgba(0, 0, 0, 0.1);
            width: 100%;
            max-width: 400px;
            text-align: center;
        }

        h1 {
            color: #cc0000;
            margin-bottom: 20px;
            font-size: 28px;
        }

        input {
            width: 100%;
            padding: 12px 15px;
            margin: 10px 0;
            border: 1px solid #ddd;
            border-radius: 6px;
            font-size: 15px;
        }

        button {
            background-color: #cc0000;
            color: white;
            border: none;
            padding: 12px 20px;
            border-radius: 6px;
            font-size: 16px;
            cursor: pointer;
            margin-top: 10px;
            width: 100%;
        }

        button:hover {
            background-color: #a80000;
        }

        .msg {
            margin-top: 20px;
            font-size: 15px;
            color: green;
        }

        .error {
            color: red;
        }

        .footer-links {
            margin-top: 20px;
        }

        .footer-links a {
            text-decoration: none;
            color: #555;
            font-size: 14px;
        }

        .footer-links a:hover {
            text-decoration: underline;
        }

        .success-links {
            margin-top: 10px;
        }

        .success-links a {
            color: #0066cc;
            text-decoration: none;
            font-weight: 500;
        }

        .success-links a:hover {
            text-decoration: underline;
        }
    </style>
</head>
<body>
    <div class="login-container">
        <h1>Login to ChickenMafia</h1>
        <form method="POST">
            <input type="text" name="username" placeholder="Username" required><br>
            <input type="password" name="password" placeholder="Password" required><br>
            <button type="submit">Login</button>
        </form>

        <?php if ($message): ?>
            <div class="msg <?= ($jwt ? '' : 'error') ?>">
                <?= htmlspecialchars($message) ?>
            </div>
            <?php if ($jwt): ?>
                <div class="success-links">
                    <p>Your JWT token is stored in a secure cookie.</p>
                    <a href="orders.php">→ View Orders</a>
                </div>
            <?php endif; ?>
        <?php endif; ?>

        <div class="footer-links">
            <a href="index.php">← Back to Home</a>
        </div>
    </div>
</body>
</html>
