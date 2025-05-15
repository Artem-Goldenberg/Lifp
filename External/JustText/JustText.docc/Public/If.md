# ``JTIf``

Simulates an if condition on a macro level.

- Parameter detecter: macro which must either 
    have a comma in it's expansion or don't have a comma
    (it's not allowed to have multiple commas)

- Parameter onTrue: will be expanded if `detecter` expands to something with a comma

- Parameter onFalse: will be expanded if `detecter` expands to anything without a comma
