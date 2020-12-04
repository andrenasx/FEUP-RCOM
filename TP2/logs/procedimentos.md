**1o passo em aula:**

- desligar todos os cabos
- ligar cabos ao E0 do tux42 43 e 44 a qualquer porta do switch
- ligar cabo S0 tux 43 a T3 na calha mais acima e um cabo de T4 a  Switch Console na calha imediatamente abaixo

---

Depois dar reset ao switch e aos tuxs:

**No gtkterm para dar reset ao switch:**
- `enable`
- `(enter password) 8nortel`
- `configure terminal`
- `no vlan 2-4094`
- `exit`
- `copy flash:tux4-clean startup-config`
- `reload`

**No terminal dar reset a cada tux:**
- `updateimage`
