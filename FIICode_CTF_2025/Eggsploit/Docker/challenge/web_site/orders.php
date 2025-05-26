<?php
ini_set('display_errors', 1);
ini_set('display_startup_errors', 1);
error_reporting(E_ALL);
require 'vendor/autoload.php';

use \Firebase\JWT\JWT;
use \Firebase\JWT\Key;

$db = new SQLite3('/var/www/html/chickenmafia.db');
$results = [];
$error = '';

// Secret key to decode the JWT token
$secretKey = "your_secret_key"; // Should be the same secret key used for encoding

// Check if JWT is present in the request
$jwt = $_COOKIE['token'] ?? '';  // Assuming the token is sent in a cookie
$decoded = null;


// If no JWT or invalid JWT, redirect to login page
$headers = new stdClass();
if (empty($jwt) || !($decoded = JWT::decode($jwt,new Key($secretKey,'HS256')))) {
    echo 'You need to be logged in';
    exit;
}

// Create the 'orders' table if it doesn't exist
$db->exec("CREATE TABLE IF NOT EXISTS orders (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    item TEXT NOT NULL,
    quantity INTEGER NOT NULL,
    customer TEXT NOT NULL
)");

if (isset($_GET['search'])) {
    $search = $_GET['search'];

    // Vulnerable query directly concatenating user input
    $query = "SELECT * FROM orders WHERE item LIKE '%$search%'";  // SQL Injection vulnerability here!
    $res = @$db->query($query);

    // Output the SQL query for debugging and error purposes
    echo "<!-- <p>Executed Query: " . htmlspecialchars($query) . "</p> -->";

    if ($res) {
        while ($row = $res->fetchArray(SQLITE3_ASSOC)) {
            $results[] = $row;
        }
    } else {
        // Display any error that might occur during query execution
        $error = "Error running query: " . $db->lastErrorMsg();
    }
}
?>

<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>Orders - ChickenMafia</title>
    <link href="https://fonts.googleapis.com/css2?family=Poppins:wght@400;600&display=swap" rel="stylesheet">
    <style>
        body {
            background-color: #f9f9f9;
            font-family: 'Poppins', sans-serif;
            margin: 0;
            padding: 0;
        }

        .container {
            max-width: 900px;
            margin: 60px auto;
            background-color: #fff;
            padding: 40px;
            border-radius: 12px;
            box-shadow: 0 10px 30px rgba(0, 0, 0, 0.08);
            text-align: center;
        }

        h1 {
            color: #cc0000;
            margin-bottom: 30px;
        }

        form input {
            width: 300px;
            padding: 12px;
            margin: 10px 10px 0 0;
            border: 1px solid #ccc;
            border-radius: 6px;
            font-size: 14px;
        }

        button {
            padding: 12px 18px;
            background-color: #cc0000;
            color: #fff;
            border: none;
            border-radius: 6px;
            font-size: 14px;
            cursor: pointer;
        }

        button:hover {
            background-color: #a80000;
        }

        .error {
            color: red;
            margin-top: 20px;
        }

        table {
            width: 100%;
            border-collapse: collapse;
            margin-top: 30px;
        }

        th, td {
            border: 1px solid #ddd;
            padding: 12px;
            text-align: center;
        }

        th {
            background-color: #f2f2f2;
        }

        a {
            display: inline-block;
            margin-top: 30px;
            text-decoration: none;
            color: #333;
            font-size: 14px;
        }

        a:hover {
            text-decoration: underline;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>ChickenMafia Orders</h1>
        <form method="GET">
            <input type="text" name="search" placeholder="Search for item (e.g. wings)" required>
            <button type="submit">Search</button>
        </form>

        <?php if ($error): ?>
            <div class="error"><?= htmlspecialchars($error) ?></div>
        <?php endif; ?>

        <?php if (!empty($results)): ?>
            <table>
                <thead>
                    <tr>
                        <th>ID</th>
                        <th>Item</th>
                        <th>Quantity</th>
                        <th>Customer</th>
                    </tr>
                </thead>
                <tbody>
                    <?php foreach ($results as $order): ?>
                        <tr>
                            <td><?= htmlspecialchars(print_r($order, true)) ?></td>
                        </tr>
                    <?php endforeach; ?>
                </tbody>
            </table>
        <?php elseif (isset($_GET['search'])): ?>
            <p>No orders found for that item.</p>
        <?php endif; ?>

        <a href="index.php">← Back to Home</a>
    </div>
</body>
</html>
