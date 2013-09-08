WITH

    WRTreeDown(WorkRequestID, RecurringParentWRID, RecurringNumber)
    AS
    (
      -- anchor member
      SELECT wr.RequestID, wr.RecurringParentWRID, wr.RecurringNumber
      FROM WorkRequests wr
        INNER JOIN Entities e ON wr.RequestID = e.EntityID
      WHERE e.[Deleted] = 0 AND wr.RequestID = @WorkRequestID AND wr.BusinessID = @BusinessID

      UNION ALL

      -- recursive member
      SELECT wr.RequestID, wr.RecurringParentWRID, wr.RecurringNumber
      FROM WorkRequests wr
        INNER JOIN Entities e ON wr.RequestID = e.EntityID
        JOIN WRTreeDown wrtree ON wr.RequestID = wrtree.RecurringParentWRID
      WHERE e.[Deleted] = 0
    ),

    WRTreeUp(WorkRequestID, RecurringParentWRID, RecurringNumber)
    AS
    (
      -- anchor member
      SELECT wr.RequestID, wr.RecurringParentWRID, wr.RecurringNumber
      FROM WorkRequests wr
        INNER JOIN Entities e ON wr.RequestID = e.EntityID
      WHERE e.[Deleted] = 0 AND wr.RequestID = @WorkRequestID AND wr.BusinessID = @BusinessID

      UNION ALL

      -- recursive member
      SELECT wr.RequestID, wr.RecurringParentWRID, wr.RecurringNumber
      FROM WorkRequests wr
        INNER JOIN Entities e ON wr.RequestID = e.EntityID
        JOIN WRTreeUp wrtree ON wr.RecurringParentWRID = wrtree.WorkRequestID
      WHERE e.[Deleted] = 0
    )

    SELECT *
    FROM WorkRequests wr
    INNER JOIN Entities e ON wr.RequestID = e.EntityID
    WHERE wr.RequestID IN
    (
        SELECT DISTINCT WorkRequestID
        FROM
        (
            SELECT RecurringNumber, WorkRequestID
            FROM WRTreeDown

            UNION ALL

            SELECT RecurringNumber, WorkRequestID
            FROM WRTreeUp
        ) AS WRTree
    )
    ORDER BY wr.RecurringNumber 
