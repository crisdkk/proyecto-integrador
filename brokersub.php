<?php
// Datos de conexión a la base de datos
$servername = "mysqlalexis";
$username = "estudiante";
$password = "univalle";
$dbname = "embebidos";

// Recibe los datos enviados por el Arduino
$fundamental = $_GET['fundamental'];
$thirdHarmonic = $_GET['thirdHarmonic'];
$fifthHarmonic = $_GET['fifthHarmonic'];
$seventhHarmonic = $_GET['seventhHarmonic'];
$ninthHarmonic = $_GET['ninthHarmonic'];

// Crea una conexión a la base de datos
$conn = new mysqli($servername, $username, $password, $dbname);

// Verifica la conexión
if ($conn->connect_error) {
  die("Connection failed: " . $conn->connect_error);
}

// Elimina los registros antiguos para mantener solo los últimos 20
$sqlDelete = "DELETE FROM `armonicos-cris` WHERE id NOT IN (
                SELECT id FROM (
                  SELECT id FROM `armonicos-cris` ORDER BY fecha DESC LIMIT 20
                ) AS T
              )";

// Ejecuta la consulta de eliminación
if ($conn->query($sqlDelete) === TRUE) {
  // Prepara la consulta SQL para insertar los nuevos datos en la tabla
  $sqlInsert = "INSERT INTO `armonicos-cris` (fundamental, thirdHarmonic, fifthHarmonic, seventhHarmonic, ninthHarmonic)
                VALUES ('$fundamental', '$thirdHarmonic', '$fifthHarmonic', '$seventhHarmonic', '$ninthHarmonic')";

  // Ejecuta la consulta de inserción
  if ($conn->query($sqlInsert) === TRUE) {
    echo "Datos insertados correctamente en la tabla.";
  } else {
    echo "Error al insertar datos: " . $conn->error;
  }
} else {
  echo "Error al eliminar registros antiguos: " . $conn->error;
}

// Cierra la conexión a la base de datos
$conn->close();
?>
