<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Lectura de Armónicos</title>
    <script src="https://cdn.plot.ly/plotly-latest.min.js"></script>
    <style>
        body {
            background-color: #f0f0f0;
            font-family: Arial, sans-serif;
            margin: 0;
            padding: 20px;
        }
        .chart-container {
            background-color: #fff;
            border-radius: 5px;
            margin-bottom: 20px;
            padding: 20px;
        }
        #reset-btn {
            background-color: #4CAF50;
            color: white;
            padding: 10px 20px;
            text-align: center;
            text-decoration: none;
            display: inline-block;
            font-size: 16px;
            margin-top: 20px;
            border-radius: 5px;
            cursor: pointer;
        }
        #reset-btn:hover {
            background-color: #45a049;
        }
    </style>
</head>
<body>
    <h1>Lectura de Armónicos</h1>

    <?php
    // Datos de conexión a la base de datos
    $servername = "mysqlalexis";
    $username = "estudiante";
    $password = "univalle";
    $dbname = "embebidos";

    // Crear conexión a la base de datos
    $conn = new mysqli($servername, $username, $password, $dbname);

    // Verificar la conexión
    if ($conn->connect_error) {
        die("Connection failed: " . $conn->connect_error);
    }

    // Consulta SQL para obtener los últimos 20 registros de la tabla
    $sql = "SELECT * FROM `armonicos-cris` ORDER BY fecha DESC LIMIT 20";
    $result = $conn->query($sql);

    // Verificar si se obtuvieron resultados
    if ($result->num_rows > 0) {
        // Obtener los datos de los armónicos
        $armonicos = array();
        while ($row = $result->fetch_assoc()) {
            $armonicos[] = array(
                floatval($row['fundamental']),
                floatval($row['thirdHarmonic']),
                floatval($row['fifthHarmonic']),
                floatval($row['seventhHarmonic']),
                floatval($row['ninthHarmonic'])
            );
        }

        // Crear un contenedor para el gráfico
        echo "<div id='chart-container' class='chart-container'></div>";
    } else {
        echo "No se encontraron registros.";
    }

    // Cerrar la conexión a la base de datos
    $conn->close();
    ?>

    <button id="reset-btn" onclick="location.reload()">Actualizar</button>

    <script>
        // Datos de los armónicos obtenidos desde PHP
        var armonicos = <?php echo json_encode($armonicos); ?>;

        // Función para graficar los datos de los armónicos
        function plotChart() {
            // Crear arreglos para los datos de cada armónico
            var x = Array.from(Array(armonicos.length).keys());
            var yFundamental = [];
            var yThirdHarmonic = [];
            var yFifthHarmonic = [];
            var ySeventhHarmonic = [];
            var yNinthHarmonic = [];

            // Extraer datos de cada registro
            armonicos.forEach(function(data) {
                yFundamental.push(data[0]);
                yThirdHarmonic.push(data[1]);
                yFifthHarmonic.push(data[2]);
                ySeventhHarmonic.push(data[3]);
                yNinthHarmonic.push(data[4]);
            });

            // Crear trazos para cada armónico
            var traceFundamental = { x: x, y: yFundamental, mode: 'lines', name: 'Fundamental' };
            var traceThirdHarmonic = { x: x, y: yThirdHarmonic, mode: 'lines', name: '3rd Harmonic' };
            var traceFifthHarmonic = { x: x, y: yFifthHarmonic, mode: 'lines', name: '5th Harmonic' };
            var traceSeventhHarmonic = { x: x, y: ySeventhHarmonic, mode: 'lines', name: '7th Harmonic' };
            var traceNinthHarmonic = { x: x, y: yNinthHarmonic, mode: 'lines', name: '9th Harmonic' };

            // Configurar diseño del gráfico
            var layout = {
                title: 'Ondas de Armónicos',
                xaxis: { title: 'Muestras' },
                yaxis: { title: 'Valor' },
                margin: { t: 50 },
                height: 500
            };

            // Graficar
            Plotly.newPlot('chart-container', [traceFundamental, traceThirdHarmonic, traceFifthHarmonic, traceSeventhHarmonic, traceNinthHarmonic], layout);
        }

        // Llamar a la función para graficar
        plotChart();
    </script>
</body>
</html>
