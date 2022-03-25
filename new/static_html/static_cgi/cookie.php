<?php
setcookie("skim", "setcookie", time() + 3600, "/");
?>
<!DOCTYPE html>
<html lang = "kr" >
<head>
<meta charset = "utf-8" >
<title> 키보드와 하루 </title>
</head>
<body>
<?php
//쿠키의 testMessage에 메시지 담기
print_r($_COOKIE);
?>

쿠키를 담은 페이지
</body>
</html>
