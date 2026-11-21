miniHTTPd

Servidor web HTTP/1.1 desarrollado en lenguaje C en Linux y sockets TCP

Para compilar: make
Para eliminar el ejecutable: make clean
Para iniciar el servidor: ./minihttpd

El servidor va a escuchar en el puerto 8080.

Para acceder desde el navegador: http://localhost:8080/

Este servidor web procesa solicitudes HTTP GET y entrega archivos estáticos,
además gestiona las conexiones mediante epoll. Utiliz identificación de tipos MIME y 
genera las respuestas HTTP.
