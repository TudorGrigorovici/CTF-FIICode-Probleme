<?php
// Hardcoded credentials
$correct_username = "foarfeca";
$correct_password = "m1su_f04rf3c4";
$flag = "FIICODE25{R0b0S4pi3ns_ar3_c00l}";

if ($_SERVER["REQUEST_METHOD"] == "POST") {
    $user = $_POST['username'] ?? '';
    $pass = $_POST['password'] ?? '';

    if ($user === $correct_username && $pass === $correct_password) {
        // Successful login -> show flag
        echo "<!DOCTYPE html>
        <html lang='ro'>
        <head>
          <meta charset='UTF-8'>
          <title>Flag!</title>
          <style>
            body {
              background-color: lightblue;
              display: flex;
              flex-direction: column;
              align-items: center;
              justify-content: center;
              min-height: 100vh;
              margin: 0;
              font-family: Arial, sans-serif;
            }
            h1 {
              margin-bottom: 20px;
            }
          </style>
        </head>
        <body>
          <h1>Schema de bombardier!</h1>
          <p>Flagul ala legendar: <strong>$flag</strong></p>
        </body>
        </html>";
        exit();
    } else {
        // Failed login -> show form again with error
        $error = "Nu esti tu foarfeca!";
    }
}
?>

<!DOCTYPE html>
<html lang="ro">
<head>
  <meta charset="UTF-8">
  <title>Fan Page Amenințarea Maimuței - Login</title>
  <style>
    body {
      background-color: lightblue;
      display: flex;
      flex-direction: column;
      align-items: center;
      justify-content: center;
      min-height: 100vh;
      margin: 0;
      font-family: Arial, sans-serif;
    }

    h1 {
      margin-bottom: 20px;
    }

    form {
      display: flex;
      flex-direction: column;
      align-items: center;
    }

    input {
      margin: 5px;
      padding: 8px;
      font-size: 16px;
    }

    .error {
      color: red;
      margin-bottom: 10px;
    }
  </style>
</head>
<body>
    <h1>Login:</h1>

    <?php if (isset($error)) { echo "<div class='error'>$error</div>"; } ?>

    <form action="admin.php" method="POST">
      <input type="text" id="username" name="username" placeholder="Username" required>
      <input type="password" id="password" name="password" placeholder="Password" required>
      <input type="submit" value="Login">
    </form>
</body>
</html>
