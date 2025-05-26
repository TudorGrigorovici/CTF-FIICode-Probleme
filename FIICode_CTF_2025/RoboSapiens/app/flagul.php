<!DOCTYPE html>
<html lang="ro">
<head>
  <meta charset="UTF-8">
  <title>Fan Page Amenințarea Maimuței</title>
  <style>
    body {
      background-color: lightblue; /* Set background to blue */
      display: flex;
      flex-direction: column;
      align-items: center;
      justify-content: center;
      min-height: 100vh;
      margin: 0;
    }

    h1 {
      margin-bottom: 20px;
    }

    h2 {
      margin-bottom: 5px;
    }

    h3 {
      margin-bottom: 2px;
    }
  </style>
</head>
<body>
    <h1>Versurile la piesa:</h1>
    <form action="/flagul.php" method="POST">
      <input type="text" id="filep" name="filep" placeholder="RoboSapiens">
      <input type="submit" value="Submit">
    </form>

    <?php
      if(isset($_POST['filep']))
      {
        $filep= "/var/www/html/resources/piese/".$_POST['filep'];

        $fp = @fopen($filep, "r");
        if (!$fp) {
            echo htmlspecialchars($filep) . " not found";
        } else {
            echo "<div>";
            while (($buffer = fgets($fp, 4096)) !== false) {
                echo htmlspecialchars($buffer) . "<br>";
            }
            echo "</div>";
    
            if (!feof($fp)) {
                echo "Error: unexpected fgets() fail\n";
            }
    
            fclose($fp);
        }
      }
    ?>
</body>
</html>