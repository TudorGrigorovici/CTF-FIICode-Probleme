<?php
require 'vendor/autoload.php';
use Firebase\JWT\JWT;
use Firebase\JWT\Key;

$secret_key = "ASIIRULS";
$jwt_expiration = 36000;

$db = new SQLite3('dev/users.db');


$message = "";

function hash_password($password) {
    return hash('sha256', $password);
}

function generate_jwt($name, $artist, $secret, $exp_secs) {
    $issuedAt = time();
    $payload = [
        'iat' => $issuedAt,
        'exp' => $issuedAt + $exp_secs,
        'name' => $name,
        'artist' => $artist
    ];
    return JWT::encode($payload, $secret, 'HS256');
}

// Register
if (isset($_POST['register'])) {
    $name = $_POST['reg_name'];
    $password = hash_password($_POST['reg_password']);
    $artist = 0;

    $stmt = $db->prepare("INSERT INTO users (name, password, artist) VALUES (:name, :password, :artist)");
    $stmt->bindValue(':name', $name, SQLITE3_TEXT);
    $stmt->bindValue(':password', $password, SQLITE3_TEXT);
    $stmt->bindValue(':artist', $artist, SQLITE3_INTEGER);

    try {
        $stmt->execute();
        $jwt = generate_jwt($name, $artist, $secret_key, $jwt_expiration);
        setcookie("token", $jwt, [
            'expires' => time() + $jwt_expiration,
            'path' => '/',
            'httponly' => true,
            'samesite' => 'Strict'
        ]);
        $message = "Registration successful! Token set.";
    } catch (Exception $e) {
        $message = "Name already taken.";
    }
}

// Login
if (isset($_POST['login'])) {
    $name = $_POST['log_name'];
    $password = hash_password($_POST['log_password']);

    $stmt = $db->prepare("SELECT * FROM users WHERE name = :name");
    $stmt->bindValue(':name', $name, SQLITE3_TEXT);
    $result = $stmt->execute()->fetchArray(SQLITE3_ASSOC);
    
    if ($result && $result['password'] === $password) {
        $jwt = generate_jwt($name, $result['artist'], $secret_key, $jwt_expiration);
        setcookie("token", $jwt, [
            'expires' => time() + $jwt_expiration,
            'path' => '/',
            'httponly' => true,
            'samesite' => 'Strict'
        ]);
        $message = "Login successful! Token set.";
    } else {
        $message = "Invalid name or password.";
    }
}
?>

<!DOCTYPE html>
<html lang="en">
<heAd>
    <meta charset="UTF-8">
    <title>VibeX</title>
</head>
<body Style="display: flex; justify-content: center; align-items: center; height: 100vh; flex-direction: column; font-family: Arial, sans-serif; background-color: #111; color: #fff; padding-top: 80px;">
    <nav style="width: 100%; background-color: #222; padding: 10px 20px; position: fixed; top: 0; left: 0; display: flex; justify-content: space-between; align-items: center;">
        <div style="color: #0f0; font-weight: bold;">VibeX</div>
        <div style="flex-grow: 1; display: flex; justify-content: center;">
            <a href="index.php" style="color: white; text-decoration: none; margin-right: 35px;">Home</a>
            <a href="profile.php" style="color: white; text-decoration: none;">Songs</a>
        </div>
    </nav>   

    <h1>VibeX</h1>
    <p><?= htmlspecialchars($message) ?></p>

    <dIv style="margin-bottom: 20px;">
        <button onclick="showForm('login')" style="margin-right: 10px;">Login</button>
        <button onclick="showForm('register')">Register</button>
    </dIv>

    <!-- Login Form -->
    <div id="loginForm">
        <foRm method="post">
            <h3>Login</h3>
            <input type="text" name="log_name" placeholder="Username" required><br><br>
            <input type="password" name="log_password" placeholder="Password" required><br><br>
            <bUtton type="submit" name="login">Login</button>
        </form>
    </div>

    <!-- Register Form -->
    <div id="registerForm" styLe="display: none;">
        <form method="post">
            <h3>Register</h3>
            <input type="text" name="reg_name" placeholder="Username" required><br><br>
            <input type="password" name="reg_password" placeholder="Password" required><br><br>
            <button type="submit" name="register">Register</button>
        </form>
    </div>

    <Script>
        function showForm(form) {
            document.getElementById('loginForm').style.display = form === 'login' ? 'block' : 'none';
            document.getElementById('registerForm').style.display = form === 'register' ? 'block' : 'none';
        }
    </script>
</body>