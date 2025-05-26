<?php
ini_set('display_errors', 1);
ini_set('display_startup_errors', 1);
error_reporting(E_ALL);

require 'vendor/autoload.php';

use Firebase\JWT\JWT;
use Firebase\JWT\Key;

$secretKey = "your_secret_key";

// Function to verify and decode JWT
function verifyJWT($jwt, $secretKey) {
    try {
        return JWT::decode($jwt, new Key($secretKey, 'HS256'));
    } catch (Exception $e) {
        error_log("JWT Error: " . $e->getMessage());
        return null;
    }
}

$jwt = $_COOKIE['token'] ?? '';
$decoded = verifyJWT($jwt, $secretKey);

// Check if decoded and user is 'admin'
if (!$decoded || !isset($decoded->username) || $decoded->username !== 'admin') {
    echo htmlspecialchars($decoded->username ?? 'Unknown') . ' - You\'re not admin';
    exit;
}

$response = '';
$jsonResponse = '';
$error = '';
$debugHeaders = '';

if ($_SERVER['REQUEST_METHOD'] === 'POST' && isset($_POST['url'])) {
    $url = trim($_POST['url']);
    $method = strtoupper(trim($_POST['method'] ?? 'GET'));
    $headersInput = trim($_POST['headers'] ?? '');
    $body = trim($_POST['body'] ?? '');

    // Basic URL filter that seems protective but can be bypassed (CTF-friendly)
    if (!preg_match('/^https?:\/\//', $url)) {
        die("Invalid protocol");
    }

    $headersArray = [];
    $hasUserAgent = false;

    if (!empty($headersInput)) {
        $lines = explode("\n", $headersInput);
        foreach ($lines as $line) {
            $line = trim($line);
            if (!empty($line)) {
                $headersArray[] = $line;
                if (stripos($line, 'User-Agent:') === 0) $hasUserAgent = true;
            }
        }
    }

    if (!$hasUserAgent) {
        $headersArray[] = 'User-Agent: SystemHealthMonitor/1.0';
    }

    $opts = [
        'http' => [
            'method' => $method,
            'header' => implode("\r\n", $headersArray),
            'ignore_errors' => true
        ]
    ];

    if (in_array($method, ['POST', 'PUT', 'PATCH'])) {
        $opts['http']['content'] = $body;
    }

    $context = stream_context_create($opts);
    $response = @file_get_contents($url, false, $context);

    if ($response === false) {
        $error = "Request failed or host not reachable.";
    } elseif (!empty($http_response_header)) {
        $debugHeaders = implode("\n", $http_response_header);
        if (strpos($http_response_header[0], "200") === false) {
            $error = "Non-200 HTTP response: " . htmlspecialchars($http_response_header[0]);
        }
    }

    // Attempt to parse as JSON
    if ($response && strpos($response, '{') === 0) {
        $decodedJson = json_decode($response, true);
        if ($decodedJson !== null) {
            $jsonResponse = json_encode($decodedJson, JSON_PRETTY_PRINT);
        }
    }
}
?>

<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>System Health Check - ChickenMafia</title>
    <link href="https://fonts.googleapis.com/css2?family=Poppins:wght@400;600&display=swap" rel="stylesheet">
    <style>
        body {
            background-color: #f4f6f9;
            font-family: 'Poppins', sans-serif;
            padding: 40px;
            margin: 0;
        }

        h1 {
            color: #cc0000;
            font-size: 26px;
            margin-bottom: 20px;
            text-align: center;
        }

        form {
            background: white;
            padding: 25px;
            border-radius: 10px;
            box-shadow: 0 0 12px rgba(0,0,0,0.05);
            max-width: 900px;
            margin: auto;
        }

        label {
            display: block;
            margin-top: 16px;
            margin-bottom: 6px;
            font-weight: 600;
        }

        input[type="text"],
        select,
        textarea {
            width: 100%;
            padding: 10px 14px;
            border: 1px solid #ccc;
            border-radius: 6px;
            font-size: 14px;
            font-family: inherit;
        }

        textarea {
            resize: vertical;
            min-height: 80px;
        }

        button {
            margin-top: 20px;
            background-color: #cc0000;
            color: white;
            border: none;
            padding: 12px 20px;
            font-size: 15px;
            border-radius: 6px;
            cursor: pointer;
        }

        button:hover {
            background-color: #a80000;
        }

        .output-section {
            margin-top: 40px;
            max-width: 1000px;
            margin-left: auto;
            margin-right: auto;
        }

        .output-box {
            background-color: #fff;
            border: 1px solid #ddd;
            padding: 20px;
            border-radius: 8px;
            max-height: 400px;
            overflow: auto;
            white-space: pre-wrap;
            word-break: break-word;
            box-shadow: 0 2px 10px rgba(0,0,0,0.05);
        }

        .error {
            color: red;
            font-weight: 600;
            margin-bottom: 10px;
        }

        .footer-link {
            text-align: center;
            margin-top: 30px;
        }

        .footer-link a {
            text-decoration: none;
            color: #555;
        }

        .footer-link a:hover {
            text-decoration: underline;
        }
    </style>
</head>
<body>

<h1>System Health Check Service</h1>

<form method="POST">
    <label for="url">Target URL:</label>
    <input type="text" name="url" id="url" placeholder="http://127.0.0.1:5000" required>

    <label for="method">HTTP Method:</label>
    <select name="method" id="method">
        <option>GET</option>
        <option>POST</option>
        <option>PUT</option>
        <option>PATCH</option>
        <option>DELETE</option>
    </select>

    <label for="headers">Headers (one per line):</label>
    <textarea name="headers" id="headers" placeholder="User-Agent: HealthCheckTool/1.0"></textarea>

    <label for="body">Request Body (optional):</label>
    <textarea name="body" id="body" placeholder="JSON, XML, or any data..."></textarea>

    <button type="submit">Send Request</button>
</form>

<?php if ($error): ?>
    <div class="output-section">
        <div class="output-box error"><?= htmlspecialchars($error) ?></div>
    </div>
<?php endif; ?>

<?php if ($response): ?>
    <div class="output-section">
        <h2>Raw Response</h2>
        <div class="output-box"><?= htmlspecialchars($response) ?></div>
    </div>

    <?php if ($jsonResponse): ?>
        <div class="output-section">
            <h2>Parsed JSON</h2>
            <div class="output-box"><?= htmlspecialchars($jsonResponse) ?></div>
        </div>
    <?php endif; ?>
<?php endif; ?>

<div class="footer-link">
    <a href="index.php">← Back to Home</a>
</div>

</body>
</html>
