# Turnstile

Turnstile은 현대적인 C++로 구현된 **System LK**(고전 시퀀트 계산법, Classical Sequent Calculus) 기반의 대화형 증명 보조기(Proof Assistant)입니다. 

---

## 사전 요구 사항

- **C++20**을 지원하는 컴파일러 (GCC, Clang 또는 MSVC)
- **CMake** 3.14 이상
- **Python** 3.x (테스트 실행용)

---

## 빌드 방법

터미널에서 아래 명령을 실행하여 프로젝트를 빌드합니다.

```bash
# 빌드 디렉터리 생성 및 빌드 설정
cmake -B build -S .

# 컴파일 및 빌드 실행
cmake --build build
```

성공적으로 빌드가 완료되면 `build/` 디렉터리에 `turnstile` 실행 파일이 생성됩니다.

---

## 사용 방법

Turnstile은 두 가지 모드로 실행할 수 있습니다.

### 1. 대화형 REPL 모드
터미널에서 직접 실행하여 대화형으로 증명 단계를 하나씩 입력하며 확인할 수 있습니다. TTY를 감지하여 올바른 입력에 대해 `[OK] Step accepted.` 메시지를 출력하고, 오류가 있을 시 해당 에러 메시지를 표시합니다.

```bash
./build/turnstile
```

* 실행 예시:
```text
=========================================
  Turnstile Proof Assistant (System LK)  
=========================================
Enter your theorem and proof steps line-by-line.
Type 'exit' to quit.

> theorem identity_law := |- A -> A
  [OK] Step accepted.
>     id A
  [OK] Step accepted.
>     implR
  [OK] Step accepted.
> qed
  [OK] Step accepted.
```

### 2. 파일 리다이렉션 모드
증명이 작성된 파일 내용을 입력으로 주어 한 번에 검증을 수행할 수 있습니다. 증명에 성공할 시 종료 코드 `0`과 함께 아무 출력도 생성되지 않으며, 증명 도중 실패나 들여쓰기 오류가 있다면 표준 에러(`std::cerr`)를 통해 실패한 라인 번호와 원인을 출력합니다.

```bash
./build/turnstile < commutation.txt
```

---

## 증명 문법 및 들여쓰기 규칙

### 1. 정리 선언 (`theorem`)
증명을 시작하기 위해 정리의 이름과 시퀀트(Sequent)를 정의합니다.
* 문법: `theorem <정리이름> := <가정들> |- <결론들>`
* 예시: `theorem commutation := A /\ B |- B /\ A`

### 2. 들여쓰기 (Indentation)
블록의 깊이에 따라 들여쓰기를 올바르게 맞춰야 합니다. **들여쓰기는 항상 공백 4칸(1 탭) 단위**로 수행됩니다.
* `theorem`은 들여쓰기 깊이 `0` (맨 첫 열)에서 시작합니다.
* `new_branch` 및 일반 규칙은 스코프 수준에 맞추어 들여쓰기를 적용해야 합니다.
* **중요**: 분기를 닫는 `end` 키워드는 내부 증명 단계보다 한 단계 내어 써서 자신을 열었던 `new_branch`와 수직으로 일치시켜야 합니다 (들여쓰기 레벨 = 부모 스코프 레벨).

### 3. 분기 처리 (`new_branch` ~ `end`)
2-premise 룰(전제가 2개 필요한 규칙, 예: `andR`, `orL`, `implL`, `cut`)을 적용하기 위해 각각의 전제(Premise) 나뭇가지를 작성할 때 사용합니다.
* `new_branch`로 새로운 스택을 생성하고 내부 내용을 작성한 뒤, `end`로 닫으면 해당 서브 골(sub-goal)의 최종 증명 결과가 부모 스택으로 합쳐집니다.

---

## 실제 증명 작성 예시

### 예시 1: 항등법칙 (`tests/valid_identity.txt`)
$$\vdash A \to A$$

```text
theorem identity_law := |- A -> A
    id A
    implR
qed
```

### 예시 2: 논리곱 교환 법칙 (`tests/valid_commutation.txt`)
$$A \land B \vdash B \land A$$

```text
theorem commutation := A /\ B |- B /\ A
    new_branch
        id B
        andL2 A
    end
    new_branch
        id A
        andL1 B
    end
    andR
qed
```

---

## 테스트 실행 방법

`tests/` 디렉터리에 다양한 유효/무효 증명 케이스가 구비되어 있습니다. 자동 검증 스크립트를 사용하여 모든 검증이 정상적으로 통과하고 예외가 의도대로 처리되는지 일괄 확인할 수 있습니다.

```bash
# 자동화 테스트 스크립트 실행
python3 tests/run_tests.py
```

성공 시 아래와 같은 리포트가 출력됩니다.
```text
=========================================
  Running Turnstile Proof Assistant Tests 
=========================================
[+] TEST PASSED: valid_identity.txt (Successfully verified proof)
[+] TEST PASSED: valid_commutation.txt (Successfully verified proof)
[+] TEST PASSED: invalid_indent.txt (Failed as expected with: 'Line 3: Indent level does not match')
[+] TEST PASSED: invalid_proof.txt (Failed as expected with: 'Line 4: Invalid proof')
[+] TEST PASSED: invalid_rule_premise.txt (Failed as expected with: 'Line 2: Invalid proof: not enough premises on stack')
[+] TEST PASSED: invalid_no_theorem.txt (Failed as expected with: 'Line 1: Provable cannot be defined in a theorem context')
=========================================
Results: 6/6 tests passed.
=========================================
[+] All tests passed successfully!
```
