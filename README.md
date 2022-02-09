# webserv
팀원: spark / skim / hybae / kkim / Juhpark

# 1. Coding Convention
## 1.1. Basic Rules
```
TAB: 4 spaces, doesn't matter tap or a space
```

## 1.2. Variable
```
Underbar at first character
Snake case (small letter only, separate word with _)
No limits for variable letter 
Recommend 3 letter (_num, _str, ...): it could be confused
Example: _num, _header_field, _mth
```

## 1.3. Class
```
Pascal Case (Start with BIG letter, separate with alphabet size)
Example: ConfigController, RequestMessage
```

## 1.4. Function
```
Camel Case  (start with small letter, separate with alphabet size)

- case 1
return
    functionName(_parameter)
{ function }

- case 2 : Simple function, getter/setter, ...
return  functionName(_parameter)    { function }
```

## 1.5. Comment Rules
```
Class explanation (1.5.1)
Function explanation? (1.5.2) : (hasn't been introduced yet, but soon)
Anything one-line comment you want to explain (//)
```
### 1.5.1. Class Explnation Example
```
/**
Header
-----------
> Class_Name : PARENT
Private variable ...
Protected variable ...
Public variable ...
Member function ...
Nonmember function ...
Private function ...
... anything you have.
함수 이름과 한줄설명, 탭으로 시작 위치 맞추기
한줄로만 간단하게 설명, 자세한 리턴이나 파라미터 관련 내용은 1.5.2.에서
*/
```

<details markdown="1">
    <summary>예시 보기</summary>
    
```
/**
-------------------------------------------------------------
> HTTPMessage

- Protected Variables:
_header_field			map(string, string), store header field
_start_line	  		    string, Save start lines
_msg_body			  	string, Save the message body

- Member functions:
Getter:				    MessageBody, StartLine
parseHeaderField		Parse header field and save to _header_field(map).
setHeaderField		    Insert new pair to _header_field
printHeaderField		We know it remains for debugging
-------------------------------------------------------------
*/
```

</details>

### 1.5.2. Function Explanation : 이건 하지 말까..?
```
함수와 탭 맞추기
/** (/*로 하면 @param, @return 등이 Highlight 처리가 안됨 vscode 기준)
간단한 설명 (1.5.1.의 설명이랑 같아도 됨)
@return       explanation
@param _var   explanation
@todo
... etc
*/
```

<details markdown="1">
    <summary>예시 보기</summary>
    
```
/**
Parse header field and save to _header_field(map).

@return         last_index + 1
@param  _msg    클라이언트 측에서 전송하는 데이터
@param  pos     start_line을 parsing하고 난 이후의 위
*/
```

</details>
