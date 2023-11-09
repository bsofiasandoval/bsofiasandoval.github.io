<?php

// Assuming these values are obtained from the GET request
$contador = $_GET['cont'];
$moving = $_GET['move']; // Make sure you have a 'temp' GET parameter being sent from the NodeMCU

// Database credentials and connection
$usuario = "root";
$contrasena = ""; // It's advised to set a password for your database user
$servidor = "localhost";
$basededatos = "sensores";

$conexion = mysqli_connect($servidor, $usuario, $contrasena, $basededatos) or die("No se ha podido conectar al servidor de base de datos");

// Prepared statement to insert data into the 'lecturas' table
$consulta = "INSERT INTO movimiento (contador, moving) VALUES (?, ?)";
$stmt = mysqli_prepare($conexion, $consulta);

// Check if the statement was prepared correctly
if ($stmt) {
    // Bind the integer value of 'contador', the float value of 'temperatura', and the string value of 'status' to the placeholders in the SQL statement
    mysqli_stmt_bind_param($stmt, "is", $contador, $moving);

    // Execute the SQL statement
    mysqli_stmt_execute($stmt);

    // Check for successful insertion
    if (mysqli_stmt_affected_rows($stmt) > 0) {
        echo "Insercion exitosa";
    } else {
        echo "Error en la insercion";
    }

    // Close the statement
    mysqli_stmt_close($stmt);
} else {
    // Handle errors with preparing the statement
    echo "Error al preparar la consulta";
}

// Close the database connection
mysqli_close($conexion);

?>