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
  () -> (A |- A)
  [OK] Step accepted.
>     implR
  (A |- A) -> ( |- (A -> A))
  [OK] Step accepted.
> qed
  [OK] Step accepted.
> exit
```

### 2. 파일 리다이렉션 모드
증명이 작성된 파일 내용을 입력으로 주어 한 번에 검증을 수행할 수 있습니다. 증명에 성공할 시 종료 코드 `0`과 함께 아무 출력도 생성되지 않으며, 증명 도중 실패나 들여쓰기 오류가 있다면 표준 에러(`std::cerr`)를 통해 실패한 라인 번호와 원인을 출력합니다.

```bash
./build/turnstile < commutation.txt
```

---

## 증명 문법 및 들여쓰기 규칙

### 1. 정리 선언 (`theorem`)
증명을 시작하기 위해 정리의 이름과 시퀀트(Sequent)를 정의합니다. 정리의 이름은 일반 식별자(Identifier) 뿐만 아니라 큰따옴표로 감싸진 문자열(String)도 허용됩니다.
* **문법**: 
  - `theorem <정리이름> := <가정들> |- <결론들>`
* **예시**:
  - `theorem commutation := A /\ B |- B /\ A`
  - `theorem "identity law" := |- A -> A`

### 2. 들여쓰기 (Indentation)
블록의 깊이에 따라 들여쓰기를 올바르게 맞춰야 합니다. **들여쓰기는 항상 공백 4칸(1 탭) 단위**로 수행됩니다.
* `theorem` 선언은 들여쓰기 깊이 `0` (맨 첫 열)에서 시작합니다.
* `new_branch` 및 일반 규칙은 스코프 수준에 맞추어 들여쓰기를 적용해야 합니다.
* **중요**: 분기를 닫는 `end` 키워드는 내부 증명 단계보다 한 단계 내어 써서 자신을 열었던 `new_branch`와 수직으로 일치시켜야 합니다 (들여쓰기 레벨 = 부모 스코프 레벨).

### 3. 분기 처리 (`new_branch` ~ `end`)
2-premise 룰(전제가 2개 필요한 규칙, 예: `andR`, `orL`, `implL`, `cut`)을 적용하기 위해 각각의 전제(Premise) 나뭇가지를 작성할 때 사용합니다.
* `new_branch`로 새로운 스택을 생성하고 내부 내용을 작성한 뒤, `end`로 닫으면 해당 서브 골(sub-goal)의 최종 증명 결과가 부모 스택으로 합쳐집니다.

---

## 💻 System LK 추론 규칙 상세 설명

Turnstile은 고전 시퀀트 계산법(System LK)을 따르며, 증명은 **하향식(Axiom에서 출발하여 Goal로 유도)**으로 스택에 시퀀트를 쌓아가는 순서로 작성됩니다.

### 1. 기본 공리 및 정리 참조 규칙 (0-Premise Rules)

| 규칙명 | 문법 | 수학적 표현 | 설명 |
| :--- | :--- | :--- | :--- |
| **Identity** | `id <F>` | $$\psi \vdash \psi$$ | 가장 기본적인 공리(Axiom)로, 동일한 식 $$\psi$$가 양변에 존재함을 선언하여 새로운 증명 줄기를 시작합니다. |
| **Use** | `use <Name>` | (이전 정리의 시퀀트) | 이전에 성공적으로 증명한 정리(Theorem)의 시퀀스를 스택에 불러옵니다. 식별자 또는 큰따옴표로 정의된 문자열 이름을 모두 참조할 수 있습니다. |

### 2. 1-전제 규칙 (1-Premise Rules)
스택의 최상단 시퀀스 하나를 팝(Pop)하여 규칙을 적용한 뒤, 그 결과를 다시 스택에 푸시(Push)합니다.

| 규칙명 | 문법 | 수학적 변환 | 설명 |
| :--- | :--- | :--- | :--- |
| **And Left 1** | `andL1 <B>` | $$\frac{\Gamma, A \vdash \Delta}{\Gamma, A \land B \vdash \Delta}$$ | 좌변의 맨 마지막 식 $$A$$를 $$A \land B$$로 확장합니다. |
| **And Left 2** | `andL2 <A>` | $$\frac{\Gamma, B \vdash \Delta}{\Gamma, A \land B \vdash \Delta}$$ | 좌변의 맨 마지막 식 $$B$$를 $$A \land B$$로 확장합니다. |
| **Or Right 1** | `orR1 <B>` | $$\frac{\Gamma \vdash A, \Delta}{\Gamma \vdash A \lor B, \Delta}$$ | 우변의 맨 첫 번째 식 $$A$$를 $$A \lor B$$로 확장합니다. |
| **Or Right 2** | `orR2 <A>` | $$\frac{\Gamma \vdash A, \Delta}{\Gamma \vdash B \lor A, \Delta}$$ | 우변의 맨 첫 번째 식 $$A$$를 $$B \lor A$$로 확장합니다. |
| **Implication Right**| `implR` | $$\frac{\Gamma, A \vdash B, \Delta}{\Gamma \vdash A \to B, \Delta}$$ | 우변에 함축($$\to$$)을 도입합니다. 좌변의 마지막 식 $$A$$가 우변의 함축 조건으로 올라갑니다. |
| **Negation Left** | `notL` | $$\frac{\Gamma \vdash A, \Delta}{\Gamma, \neg A \vdash \Delta}$$ | 우변의 첫 번째 식 $$A$$를 부정($$\sim$$)하여 좌변의 마지막 식으로 이동시킵니다. |
| **Negation Right** | `notR` | $$\frac{\Gamma, A \vdash \Delta}{\Gamma \vdash \neg A, \Delta}$$ | 좌변의 마지막 식 $$A$$를 부정($$\sim$$)하여 우변의 첫 번째 식으로 이동시킵니다. |

### 3. 2-전제 규칙 (2-Premise Rules)
스택에 연속해서 쌓여 있는 두 개의 시퀀스를 모두 팝(Pop)하여 결합 규칙을 적용합니다. 이를 위해 증명 내에서 `new_branch` ~ `end` 구조를 사용해야 합니다.

| 규칙명 | 문법 | 수학적 변환 | 설명 |
| :--- | :--- | :--- | :--- |
| **Or Left** | `orL` | $$\frac{\Gamma, A \vdash \Delta \quad \Gamma, B \vdash \Delta}{\Gamma, A \lor B \vdash \Delta}$$ | 좌변에 선언($$\lor$$)을 도입합니다. 두 전제의 $$\Gamma$$와 $$\Delta$$가 완전히 일치해야 합니다. |
| **And Right** | `andR` | $$\frac{\Gamma \vdash A, \Delta \quad \Gamma \vdash B, \Delta}{\Gamma \vdash A \land B, \Delta}$$ | 우변에 논리곱($$\land$$)을 도입합니다. 두 전제의 $$\Gamma$$와 $$\Delta$$가 완전히 일치해야 합니다. |
| **Implication Left**| `implL` | $$\frac{\Gamma \vdash A, \Delta \quad \Sigma, B \vdash \Pi}{\Gamma, \Sigma, A \to B \vdash \Delta, \Pi}$$ | 두 시퀀스를 결합하면서 함축($$\to$$)을 좌변에 만듭니다. |
| **Cut** | `cut` | $$\frac{\Gamma \vdash \Delta, A \quad A, \Sigma \vdash \Pi}{\Gamma, \Sigma \vdash \Delta, \Pi}$$ | 컷(Cut) 규칙을 통해 중간 매개식 $$A$$를 소거하여 결론을 유도합니다. |

### 4. 1차 술어 논리 양화사 규칙 (First-Order Quantifier Rules)

| 규칙명 | 문법 | 수학적 변환 | 설명 |
| :--- | :--- | :--- | :--- |
| **Forall Left** | `forallL <t> <x>` | $$\frac{\Gamma, A[t/x] \vdash \Delta}{\Gamma, \forall x. A \vdash \Delta}$$ | 특수화된 예시 식 $$A[t/x]$$로부터 일반화된 전칭 양화사($$\forall x. A$$) 식을 유도합니다. |
| **Exist Left** | `existL <y> <x>` | $$\frac{\Gamma, A[y/x] \vdash \Delta}{\Gamma, \exists x. A \vdash \Delta}$$ | 존재 양화사 제거 규칙입니다. 변수 $$y$$는 전제 시퀀스의 $$\Gamma$$, $$\Delta$$에 등장하지 않는 새로운 변수여야 합니다 (**Eigenvariable Condition**). |
| **Forall Right** | `forallR <y> <x>` | $$\frac{\Gamma \vdash A[y/x], \Delta}{\Gamma \vdash \forall x. A, \Delta}$$ | 임의의 변수 $$y$$에 대한 참으로부터 전칭 도입을 합니다. 변수 $$y$$는 $$\Gamma$$, $$\Delta$$에 등장할 수 없습니다 (**Eigenvariable Condition**). |
| **Exist Right** | `existR <t> <x>` | $$\frac{\Gamma \vdash A[t/x], \Delta}{\Gamma \vdash \exists x. A, \Delta}$$ | 특정한 대상 $$t$$에 대해 참인 식으로부터 존재 양화사($$\exists x. A$$)를 도입합니다. |

### 5. 구조적 추론 규칙 (Structural Rules)
시퀀트의 양변을 정리하거나 불필요한 식을 추가/삭제/변경하는 규칙입니다.

| 규칙명 | 문법 | 수학적 변환 | 설명 |
| :--- | :--- | :--- | :--- |
| **Weakening Left** | `wl <F>` | $$\frac{\Gamma \vdash \Delta}{\Gamma, F \vdash \Delta}$$ | 좌변의 맨 마지막에 불필요하거나 새로운 가정 $$F$$를 임의로 추가합니다. |
| **Weakening Right**| `wr <F>` | $$\frac{\Gamma \vdash \Delta}{\Gamma \vdash F, \Delta}$$ | 우변의 맨 첫 자리에 새로운 결론 후보 $$F$$를 임의로 추가합니다. |
| **Contraction Left**| `cl` | $$\frac{\Gamma, A, A \vdash \Delta}{\Gamma, A \vdash \Delta}$$ | 좌변 맨 끝에 연속한 두 개의 동일한 가정 $$A$$를 하나로 축소합니다. |
| **Contraction Right**| `cr` | $$\frac{\Gamma \vdash A, A, \Delta}{\Gamma \vdash A, \Delta}$$ | 우변 맨 앞의 두 개의 동일한 결론 $$A$$를 하나로 축소합니다. |
| **Permutation Left**| `pl <i>` | $$\frac{\Gamma, A, B, \Lambda \vdash \Delta}{\Gamma, B, A, \Lambda \vdash \Delta}$$ | 좌변에서 인덱스 `i`와 `i+1` 위치의 식의 순서를 교환합니다. |
| **Permutation Right**| `pr <i>` | $$\frac{\Gamma \vdash \Lambda, A, B, \Delta}{\Gamma \vdash \Lambda, B, A, \Delta}$$ | 우변에서 인덱스 `i`와 `i+1` 위치의 식의 순서를 교환합니다. |

---

## 📝 실제 증명 작성 예시

### 예시 1: 항등법칙 (`tests/valid_identity.txt`)
$$\vdash A \to A$$

```text
theorem identity_law := |- A -> A
    id A
    implR
qed
```

### 예시 2: 정리 사용 (`tests/valid_use.txt`)
이전에 성공한 정리를 `use`를 통해 임포트하여 함축과 문자열 정리를 증명합니다.

```text
theorem identity_law := |- A -> A
    id A
    implR
qed

theorem "contra law" := A -> B |- ~B -> ~A
    id A
    new_branch
        id B
        notL
        pl 0
    end
    implL
    pl 1
    pl 0
    pl 1
    notR
    implR
qed

theorem use_test := |- A -> A
    use identity_law
qed

theorem use_string_test := A -> B |- ~B -> ~A
    use "contra law"
qed
```

---

## 🧪 테스트 실행 방법

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
[+] TEST PASSED: valid_distributivity.txt (Successfully verified proof)
[+] TEST PASSED: valid_contraposition.txt (Successfully verified proof)
[+] TEST PASSED: valid_use.txt (Successfully verified proof)
[+] TEST PASSED: invalid_use.txt (Failed as expected with: 'Line 2: Invalid Use application')
[+] TEST PASSED: invalid_indent.txt (Failed as expected with: 'Line 3: Indent level does not match')
[+] TEST PASSED: invalid_proof.txt (Failed as expected with: 'Line 4: Invalid proof')
[+] TEST PASSED: invalid_rule_premise.txt (Failed as expected with: 'Line 2: Invalid proof: not enough premises on stack')
[+] TEST PASSED: invalid_no_theorem.txt (Failed as expected with: 'Line 1: Provable cannot be defined in a theorem context')
=========================================
Results: 10/10 tests passed.
=========================================
[+] All tests passed successfully!
```
