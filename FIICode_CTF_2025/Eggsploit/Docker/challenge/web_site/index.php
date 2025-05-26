<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>ChickenMafia - Home</title>
    <link href="https://fonts.googleapis.com/css2?family=Poppins:wght@400;600&display=swap" rel="stylesheet">
    <style>
        body {
            margin: 0;
            padding: 0;
            background-color: #fafafa;
            font-family: 'Poppins', sans-serif;
            display: flex;
            align-items: center;
            justify-content: center;
            min-height: 100vh;
        }

        .container {
            background-color: #fff;
            padding: 40px 60px;
            border-radius: 12px;
            box-shadow: 0 10px 25px rgba(0,0,0,0.1);
            text-align: center;
            max-width: 500px;
        }

        h1 {
            color: #cc0000;
            font-size: 32px;
            margin-bottom: 10px;
        }

        p {
            color: #555;
            font-size: 16px;
            margin-bottom: 30px;
        }

        .actions a {
            display: inline-block;
            margin: 10px 15px;
            padding: 10px 20px;
            background-color: #cc0000;
            color: #fff;
            border-radius: 6px;
            text-decoration: none;
            transition: background-color 0.3s ease;
            font-weight: 600;
        }

        .actions a:hover {
            background-color: #a80000;
        }

        .logo {
            max-width: 220px;
            margin-top: 30px;
            border-radius: 10px;
        }

        .tagline {
            font-size: 14px;
            color: #888;
            margin-top: 10px;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>ChickenMafia</h1>
        <p>Where cravings meet their match — wings, drumsticks, and more. Fast. Crispy. Legendary.</p>

        <div class="actions">
            <a href="login.php">Login</a>
            <a href="register.php">Register</a>
        </div>

        <img class="logo" src="images/chickenMafia.jpeg" alt="ChickenMafia Logo">
        <div class="tagline">Your favorite underground chicken syndicate.</div>
    </div>
</body>
</html>
