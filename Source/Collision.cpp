#include "Collision.h"
#include <algorithm>
using namespace DirectX;
// 球と球の交差判定
bool Collision::IntersectSphereVsSphere(
	const DirectX::XMFLOAT3& positionA,
	float radiusA,
	const DirectX::XMFLOAT3& positionB,
	float radiusB,
	DirectX::XMFLOAT3& outVec
)
{
    DirectX::XMVECTOR posA = DirectX::XMLoadFloat3(&positionA);
    DirectX::XMVECTOR posB = DirectX::XMLoadFloat3(&positionB);
    // ２点間の位置ベクトル引く
    DirectX::XMVECTOR vec = DirectX::XMVectorSubtract(posB, posA);
    // 単一の長さを計算
    DirectX::XMVECTOR length = DirectX::XMVector3Length(vec);
    // float 型に変換
    float len;
    DirectX::XMStoreFloat(&len, length);

    // 距離判定
    float range = radiusA + radiusB;
    if (len > range)
    {
        return false;
    }

    // 衝突していたら衝突方向のベクトルを設定
    vec = DirectX::XMVector3Normalize(vec);
    DirectX::XMStoreFloat3(&outVec, vec);

    return true;
}

// 円柱と円柱の交差判定
bool Collision::IntersectCylinderVsCylinder(
    const DirectX::XMFLOAT3& positionA,
    float radiusA,
    float heightA,
    const DirectX::XMFLOAT3& positionB,
    float radiusB,
    float heightB,
    DirectX::XMFLOAT3& outVec)
{
    // A の足もとが B の頭より上なら衝突していない
    if (positionA.y > positionB.y + heightB)
        return false;

    // A の頭が B の足もとより下なら衝突していない
    if (positionA.y + heightA < positionB.y)
        return false;

    // XZ 平面での範囲チェック
    float vx = positionB.x - positionA.x;
    float vz = positionB.z - positionA.z;
    float range = radiusA + radiusB;
    float distXZ = sqrtf(vx * vx + vz * vz);
    if (distXZ > range)
        return false;

    // 衝突方向のベクトル
    outVec.x = vx / distXZ;
    outVec.y = 0.0f;
    outVec.z = vz / distXZ;

    return true;
}

bool Collision::IntersectCapsuleVsCapsule(
    const DirectX::XMVECTOR& position1,	// 中心
    const DirectX::XMVECTOR& direction1,	// 向き（正規化）
    const float					length1,	// 長さ
    const float					radius1,	// 半径
    const DirectX::XMVECTOR& position2,	// 中心
    const DirectX::XMVECTOR& direction2,	// 向き（正規化）
    const float					length2,	// 長さ
    const float					radius2,	// 半径
    IntersectionResult* result)
{


    // 各カプセルの中心線上の最近点算出
    //向きの方向にしか伸びてないからこれはだめっだった
    DirectX::XMVECTOR point1 = {}, point2 = {};
    DirectX::XMVECTOR end1 = DirectX::XMVectorAdd(position1, DirectX::XMVectorScale(direction1, length1));
    DirectX::XMVECTOR end2 = DirectX::XMVectorAdd(position2, DirectX::XMVectorScale(direction2, length2));

    // 線分同士の最短距離の二乗を取得
    float distSq = GetMinDistSq_SegmentSegment(position1, end1, position2, end2, &point1, &point2);
    DirectX::XMVECTOR vec = DirectX::XMVectorSubtract(point1, point2);
    float radiusAdd = radius1 + radius2;

    // 最近接点間の距離がカプセルの半径の和より小さい場合、衝突している
    if (distSq < radiusAdd * radiusAdd)
    {
        if (result)
        {
            result->normal = DirectX::XMVector3Normalize(vec);
            result->penetration = radiusAdd - sqrtf(distSq);
            result->pointA = DirectX::XMVectorAdd(point1, DirectX::XMVectorScale(result->normal, -radius1));
            result->pointB = DirectX::XMVectorAdd(point2, DirectX::XMVectorScale(result->normal, radius2));
        }

        return true;
    }

    return false;
}

// 線分と線分の最短距離の二乗を取得する
float Collision::GetMinDistSq_SegmentSegment(
    const  DirectX::XMVECTOR& point1A,
    const  DirectX::XMVECTOR& point1B,
    const  DirectX::XMVECTOR& point2A,
    const  DirectX::XMVECTOR& point2B,
    DirectX::XMVECTOR* nearPoint1,
    DirectX::XMVECTOR* nearPoint2)
{
    DirectX::XMVECTOR segmentDirection1 = DirectX::XMVectorSubtract(point1B, point1A);
    DirectX::XMVECTOR segmentDirection2 = DirectX::XMVectorSubtract(point2B, point2A);
    DirectX::XMVECTOR r = DirectX::XMVectorSubtract(point1A, point2A);

    float a = DirectX::XMVectorGetX(DirectX::XMVector3Dot(segmentDirection1, segmentDirection1));
    float e = DirectX::XMVectorGetX(DirectX::XMVector3Dot(segmentDirection2, segmentDirection2));
    float f = DirectX::XMVectorGetX(DirectX::XMVector3Dot(segmentDirection2, r));

    float t1 = 0.0f, t2 = 0.0f;

    if (a <= FLT_EPSILON && e <= FLT_EPSILON)	// 両線分が点に縮退している場合
    {
        t1 = t2 = 0.0f;
    }
    else if (a <= FLT_EPSILON)					// 片方（d0）が点に縮退している場合
    {
        t1 = 0.0f;
        t2 = std::clamp(f / e, 0.0f, 1.0f);
    }
    else
    {
        float c = DirectX::XMVectorGetX(DirectX::XMVector3Dot(segmentDirection1, r));
        if (e <= FLT_EPSILON)					// 片方（d1）が点に縮退している場合
        {
            t2 = 0.0f;
            t1 = std::clamp(-c / a, 0.0f, 1.0f);
        }
        else									// 両方が線分
        {
            float b = DirectX::XMVectorGetX(DirectX::XMVector3Dot(segmentDirection1, segmentDirection2));
            float denom = a * e - b * b;

            if (denom != 0.0f)					// 平行確認（平行時は t1 = 0.0f（線分の始端）を仮の初期値として計算をすすめる）
            {
                t1 = std::clamp((b * f - c * e) / denom, 0.0f, 1.0f);
            }

            t2 = b * t1 + f;

            if (t2 < 0.0f)						// t1が始端より外側にある場合
            {
                t2 = 0.0f;
                t1 = std::clamp(-c / a, 0.0f, 1.0f);
            }
            else if (t2 > e)					// t1が終端より外側にある場合
            {
                t2 = 1.0f;
                t1 = std::clamp((b - c) / a, 0.0f, 1.0f);
            }
            else								// t1が線分上にある場合
            {
                t2 /= e;
            }
        }
    }

    // 各線分上の最近点算出
    DirectX::XMVECTOR point1 = DirectX::XMVectorAdd(point1A, DirectX::XMVectorScale(segmentDirection1, t1));
    DirectX::XMVECTOR point2 = DirectX::XMVectorAdd(point2A, DirectX::XMVectorScale(segmentDirection2, t2));

    DirectX::XMVECTOR vec = DirectX::XMVectorSubtract(point1, point2);

    if (nearPoint1)
    {
        *nearPoint1 = point1;
    }
    if (nearPoint2)
    {
        *nearPoint2 = point2;
    }

    return DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(vec));
}

// 球と円柱の交差反映
bool Collision::IntersectSphereVsCylinder(
    const DirectX::XMFLOAT3& positionA,
    float radiusA,
    const DirectX::XMFLOAT3& positionB,
    float radiusB,
    float heightB,
    DirectX::XMFLOAT3& outVec)
{
    // A（球）の足もとが B（円柱）の頭より上なら衝突していない
    if (positionA.y - radiusA > positionB.y + heightB)
        return false;

    // A（球）の頭が B（円柱）の足もとより下なら衝突していない
    if (positionA.y + radiusA < positionB.y)
        return false;

    // XZ 平面での範囲チェック
    float vx = positionB.x - positionA.x;
    float vz = positionB.z - positionA.z;
    float range = radiusA + radiusB;
    float distXZ = sqrtf(vx * vx + vz * vz);
    if (distXZ > range)
        return false;

    // 衝突方向のベクトル
    outVec.x = vx / distXZ;
    outVec.y = 0.0f;
    outVec.z = vz / distXZ;

    return true;
}


bool Collision::RayIntersectsTriangle(
    const DirectX::XMFLOAT3& rayStart,
    const DirectX::XMFLOAT3& rayEnd,
    const DirectX::XMFLOAT3& a,
    const DirectX::XMFLOAT3& b,
    const DirectX::XMFLOAT3& c,
    HitResult& result)
{
    using namespace DirectX;

    // レイの始点と終点をDirectX::XMVECTOR型に変換
    XMVECTOR rayOrigin = XMLoadFloat3(&rayStart);
    XMVECTOR rayDir = XMLoadFloat3(&rayEnd) - rayOrigin; // レイの方向ベクトル

    // 三角形の頂点をDirectX::XMVECTOR型に変換
    XMVECTOR vertexA = XMLoadFloat3(&a);
    XMVECTOR vertexB = XMLoadFloat3(&b);
    XMVECTOR vertexC = XMLoadFloat3(&c);

    // 三角形の辺ベクトルを計算
    XMVECTOR edgeAB = vertexB - vertexA;
    XMVECTOR edgeAC = vertexC - vertexA;

    // レイと三角形の法線の計算
    XMVECTOR h = XMVector3Cross(rayDir, edgeAC);
    float aVal = XMVectorGetX(XMVector3Dot(edgeAB, h));

    // レイと三角形が平行な場合、交差しない
    if (aVal > -FLT_EPSILON && aVal < FLT_EPSILON)
        return false;

    // 逆数を計算
    float f = 1.0f / aVal;

    // レイの始点と三角形の頂点Aの差ベクトルを計算
    XMVECTOR s = rayOrigin - vertexA;

    // u座標の計算
    float u = f * XMVectorGetX(XMVector3Dot(s, h));
    if (u < 0.0f || u > 1.0f)
        return false;

    // v座標の計算
    XMVECTOR q = XMVector3Cross(s, edgeAB);
    float v = f * XMVectorGetX(XMVector3Dot(rayDir, q));
    if (v < 0.0f || u + v > 1.0f)
        return false;

    // t（交差点までの距離）の計算
    float t = f * XMVectorGetX(XMVector3Dot(edgeAC, q));

    // tが正であれば交差している
    if (t > FLT_EPSILON)
    {
        // 交点の座標を計算
        XMVECTOR hitPos = rayOrigin + rayDir * t;
        XMStoreFloat3(&result.position, hitPos);

        // 三角形の法線を計算
        XMVECTOR normal = XMVector3Normalize(XMVector3Cross(edgeAB, edgeAC));
        XMStoreFloat3(&result.normal, normal);

        // 距離を保存
        result.distance = t;

        return true; // 交差した
    }

    return false; // 交差しない
}


// レイとモデルの交差判定
bool Collision::IntersectRayVsModel(
    const DirectX::XMFLOAT3& start,
    const DirectX::XMFLOAT3& end,
    const Model* model,
    HitResult& result)
{
#if 0
    // ワールド空間上でのレイの始点
    DirectX::XMVECTOR worldRayStartVec = DirectX::XMLoadFloat3(&start);
    // ワールド空間上でのレイの終点
    DirectX::XMVECTOR worldRayEndVec = DirectX::XMLoadFloat3(&end);
    // ワールド空間上でのレイの始点から終点までのベクトル
    DirectX::XMVECTOR worldRayVec = DirectX::XMVectorSubtract(worldRayEndVec, worldRayStartVec);
    // ワールド空間上でのレイの長さ
    DirectX::XMVECTOR worldRayLength = DirectX::XMVector3Length(worldRayVec);
    DirectX::XMStoreFloat(&result.distance, worldRayLength);

    // true...衝突した
    bool hit = false;
    for (const Model::Mesh& mesh_ : model->meshes)
    {
        // レイをワールド空間からローカル空間へ変換
        DirectX::XMMATRIX worldTransformMat = DirectX::XMLoadFloat4x4(&mesh_.defaultGlobalTransform);
        DirectX::XMMATRIX inverseWorldTransformMat = DirectX::XMMatrixInverse(nullptr, worldTransformMat);

        // ローカル空間上でのレイの始点
        DirectX::XMVECTOR localRayStartVec = DirectX::XMVector3TransformCoord(worldRayStartVec, inverseWorldTransformMat);
        // ローカル空間上でのレイの終点
        DirectX::XMVECTOR localRayEndVec = DirectX::XMVector3TransformCoord(worldRayEndVec, inverseWorldTransformMat);
        // ローカル空間上でのレイの始点から終点までのベクトル
        DirectX::XMVECTOR localRayVec = DirectX::XMVectorSubtract(localRayEndVec, localRayStartVec);
        // ローカル空間上でのレイの方向ベクトル
        DirectX::XMVECTOR localRayDirectVec = DirectX::XMVector3Normalize(localRayVec);
        // ローカル空間上でのレイの長さ
        DirectX::XMVECTOR localRayLengthVec = DirectX::XMVector3Length(localRayVec);
        float localRayLength;
        DirectX::XMStoreFloat(&localRayLength, localRayLengthVec);

        // 頂点データを取得
        const std::vector<Model::vertex>& vertices = mesh_.vertices;
        const std::vector<UINT>& indices = mesh_.indices;

        // 候補となる情報
        int materialIndex = -1;
        DirectX::XMVECTOR hitPosition;
        DirectX::XMVECTOR hitNormal;

        for (const Model::Mesh::Subset& subset : mesh_.subsets)
        {
            for (UINT i = 0; i < subset.indexCount; i += 3)
            {
                UINT index = subset.startIndexLocation + i;

                // 三角形の頂点の抽出
                const Model::vertex& a = vertices.at(indices.at(index));
                const Model::vertex& b = vertices.at(indices.at(index + 1));
                const Model::vertex& c = vertices.at(indices.at(index + 2));

                DirectX::XMVECTOR aVec = DirectX::XMLoadFloat3(&a.position);
                DirectX::XMVECTOR bVec = DirectX::XMLoadFloat3(&b.position);
                DirectX::XMVECTOR cVec = DirectX::XMLoadFloat3(&c.position);

                DirectX::XMVECTOR abVec = DirectX::XMVectorSubtract(bVec, aVec);
                DirectX::XMVECTOR bcVec = DirectX::XMVectorSubtract(cVec, bVec);
                DirectX::XMVECTOR caVec = DirectX::XMVectorSubtract(aVec, cVec);

                // 三角形の法線を計算
                DirectX::XMVECTOR normalVec = DirectX::XMVector3Cross(abVec, bcVec);

                // レイの方向と面の方向をチェック（平面の表裏判定）
                DirectX::XMVECTOR dotVec = DirectX::XMVector3Dot(localRayDirectVec, normalVec);
                float dot;
                DirectX::XMStoreFloat(&dot, dotVec);
                if (dot >= 0)
                {
                    continue;
                }

                // レイの始点から交点までの長さを計算
                DirectX::XMVECTOR tmp = DirectX::XMVectorSubtract(aVec, localRayStartVec);
                float length = DirectX::XMVectorGetX(DirectX::XMVector3Dot(tmp, normalVec)) / dot;

                // 長さが 0 以下なら以降の処理は飛ばす
                if (length < 0.0f)
                {
                    continue;
                }

                // 交点までの距離が今までに計算した最近距離より大きい時はスキップ
                if (length > localRayLength)
                {
                    continue;
                }

                // レイと平面の交点を計算
                DirectX::XMVECTOR hitPosVec = DirectX::XMVectorAdd(localRayStartVec, DirectX::XMVectorScale(localRayDirectVec, length));

                // 交点が三角形の内側にあるかどうか判定
                // 交点から点 A までのベクトル
                DirectX::XMVECTOR paVec = DirectX::XMVectorSubtract(aVec, hitPosVec);
                // 交点から点 A までのベクトルと点A点Bのベクトルの外積
                DirectX::XMVECTOR crossVec1 = DirectX::XMVector3Cross(paVec, abVec);
                // 外積と法線の向きをチェック
                DirectX::XMVECTOR dotVec1 = DirectX::XMVector3Dot(crossVec1, normalVec);
                float dot1;
                DirectX::XMStoreFloat(&dot1, dotVec1);
                if (dot1 < 0.0f)
                {
                    continue;
                }

                // 交点から点 B までのベクトル
                DirectX::XMVECTOR pbVec = DirectX::XMVectorSubtract(bVec, hitPosVec);
                // 交点から点 B までのベクトルと点B点Cのベクトルの外積
                DirectX::XMVECTOR crossVec2 = DirectX::XMVector3Cross(pbVec, bcVec);
                // 外積と法線の向きをチェック
                DirectX::XMVECTOR dotVec2 = DirectX::XMVector3Dot(crossVec2, normalVec);
                float dot2;
                DirectX::XMStoreFloat(&dot2, dotVec2);
                if (dot2 < 0.0f)
                {
                    continue;
                }

                // 交点から点 C までのベクトル
                DirectX::XMVECTOR pcVec = DirectX::XMVectorSubtract(cVec, hitPosVec);
                // 交点から点 C までのベクトルと点C点Aのベクトルの外積
                DirectX::XMVECTOR crossVec3 = DirectX::XMVector3Cross(pcVec, caVec);
                // 外積と法線の向きをチェック
                DirectX::XMVECTOR dotVec3 = DirectX::XMVector3Dot(crossVec3, normalVec);
                float dot3;
                DirectX::XMStoreFloat(&dot3, dotVec3);
                if (dot3 < 0.0f)
                {
                    continue;
                }

                // 最近距離を更新
                localRayLength = length;

                // マテリアル番号を更新
                materialIndex = subset.materialUniqueID;

                // 交点と法線を更新
                hitPosition = hitPosVec;
                hitNormal = normalVec;
            }
        }

        if (materialIndex >= 0)
        {
            // 交点座標をローカル空間からワールド空間へ変換
            DirectX::XMVECTOR worldPositionVec = DirectX::XMVector3TransformCoord(hitPosition, worldTransformMat);
            // ワールド空間上でのレイの始点から交点までのベクトル
            DirectX::XMVECTOR worldVec = DirectX::XMVectorSubtract(worldPositionVec, worldRayStartVec);
            // ワールド空間上でのレイの視点から交点までの長さ
            DirectX::XMVECTOR worldLengthVec = DirectX::XMVector3Length(worldVec);
            float distance;
            DirectX::XMStoreFloat(&distance, worldLengthVec);

            // ヒット結果情報保存
            if (result.distance > distance)
            {
                // ヒット時の面の法線をローカル空間からワールド空間へ変換
                DirectX::XMVECTOR worldNormal = DirectX::XMVector3TransformNormal(hitNormal, worldTransformMat);

                result.distance = distance;
                result.materialIndex = materialIndex;
                DirectX::XMStoreFloat3(&result.position, worldPositionVec);
                DirectX::XMStoreFloat3(&result.normal, DirectX::XMVector3Normalize(worldNormal));
                hit = true;

            }
        }
    }
    return hit;
#endif
    // ワールド空間上でのレイの始点
    DirectX::XMVECTOR worldRayStartVec = DirectX::XMLoadFloat3(&start);
    // ワールド空間上でのレイの終点
    DirectX::XMVECTOR worldRayEndVec = DirectX::XMLoadFloat3(&end);
    // ワールド空間上でのレイの始点から終点までのベクトル
    DirectX::XMVECTOR worldRayVec = DirectX::XMVectorSubtract(worldRayEndVec, worldRayStartVec);
    // ワールド空間上でのレイの長さ
    DirectX::XMVECTOR worldRayLength = DirectX::XMVector3Length(worldRayVec);
    DirectX::XMStoreFloat(&result.distance, worldRayLength);

    // true...衝突した
    bool hit = false;

    // メッシュごとに処理を行う
    for (const Model::Mesh& mesh_ : model->meshes)
    {
        // AABBの最小・最大点を取得
        DirectX::XMFLOAT3 meshMin = mesh_.boundingBox[0];
        DirectX::XMFLOAT3 meshMax = mesh_.boundingBox[1];

        // AABBの交差判定
        float tMin = 0.0f, tMax = 1.0f;
        AABB meshAABB = { meshMin, meshMax };
        if (!meshAABB.IntersectsRay(start, end, tMin, tMax))
        {
            continue;  // AABBとレイが交差しない場合、このメッシュをスキップ
        }

        // AABBと交差する場合に三角形との交差判定を行う
        DirectX::XMMATRIX worldTransformMat = DirectX::XMLoadFloat4x4(&mesh_.defaultGlobalTransform);
        DirectX::XMMATRIX inverseWorldTransformMat = DirectX::XMMatrixInverse(nullptr, worldTransformMat);

        // ローカル空間でのレイの始点と終点を計算
        DirectX::XMVECTOR localRayStartVec = DirectX::XMVector3TransformCoord(worldRayStartVec, inverseWorldTransformMat);
        DirectX::XMVECTOR localRayEndVec = DirectX::XMVector3TransformCoord(worldRayEndVec, inverseWorldTransformMat);
        DirectX::XMVECTOR localRayVec = DirectX::XMVectorSubtract(localRayEndVec, localRayStartVec);
        DirectX::XMVECTOR localRayDirectVec = DirectX::XMVector3Normalize(localRayVec);
        DirectX::XMVECTOR localRayLengthVec = DirectX::XMVector3Length(localRayVec);
        float localRayLength;
        DirectX::XMStoreFloat(&localRayLength, localRayLengthVec);

        // 頂点データを取得
        const std::vector<Model::vertex>& vertices = mesh_.vertices;
        const std::vector<UINT>& indices = mesh_.indices;

        // 候補となる情報
        int materialIndex = -1;
        DirectX::XMVECTOR hitPosition;
        DirectX::XMVECTOR hitNormal;

        for (const Model::Mesh::Subset& subset : mesh_.subsets)
        {
            for (UINT i = 0; i < subset.indexCount; i += 3)
            {
                UINT index = subset.startIndexLocation + i;

                // 三角形の頂点の抽出
                const Model::vertex& a = vertices.at(indices.at(index));
                const Model::vertex& b = vertices.at(indices.at(index + 1));
                const Model::vertex& c = vertices.at(indices.at(index + 2));

                DirectX::XMVECTOR aVec = DirectX::XMLoadFloat3(&a.position);
                DirectX::XMVECTOR bVec = DirectX::XMLoadFloat3(&b.position);
                DirectX::XMVECTOR cVec = DirectX::XMLoadFloat3(&c.position);

                DirectX::XMVECTOR abVec = DirectX::XMVectorSubtract(bVec, aVec);
                DirectX::XMVECTOR bcVec = DirectX::XMVectorSubtract(cVec, bVec);
                DirectX::XMVECTOR caVec = DirectX::XMVectorSubtract(aVec, cVec);

                // 三角形の法線を計算
                DirectX::XMVECTOR normalVec = DirectX::XMVector3Cross(abVec, bcVec);

                // レイの方向と面の方向をチェック（平面の表裏判定）
                DirectX::XMVECTOR dotVec = DirectX::XMVector3Dot(localRayDirectVec, normalVec);
                float dot;
                DirectX::XMStoreFloat(&dot, dotVec);
                if (dot >= 0)
                {
                    continue;
                }

                // レイの始点から交点までの長さを計算
                DirectX::XMVECTOR tmp = DirectX::XMVectorSubtract(aVec, localRayStartVec);
                float length = DirectX::XMVectorGetX(DirectX::XMVector3Dot(tmp, normalVec)) / dot;

                // 長さが 0 以下なら以降の処理は飛ばす
                if (length < 0.0f)
                {
                    continue;
                }

                // 交点までの距離が今までに計算した最近距離より大きい時はスキップ
                if (length > localRayLength)
                {
                    continue;
                }

                // レイと平面の交点を計算
                DirectX::XMVECTOR hitPosVec = DirectX::XMVectorAdd(localRayStartVec, DirectX::XMVectorScale(localRayDirectVec, length));

                // 交点が三角形の内側にあるかどうか判定
                DirectX::XMVECTOR paVec = DirectX::XMVectorSubtract(aVec, hitPosVec);
                DirectX::XMVECTOR crossVec1 = DirectX::XMVector3Cross(paVec, abVec);
                DirectX::XMVECTOR dotVec1 = DirectX::XMVector3Dot(crossVec1, normalVec);
                float dot1;
                DirectX::XMStoreFloat(&dot1, dotVec1);
                if (dot1 < 0.0f)
                {
                    continue;
                }

                DirectX::XMVECTOR pbVec = DirectX::XMVectorSubtract(bVec, hitPosVec);
                DirectX::XMVECTOR crossVec2 = DirectX::XMVector3Cross(pbVec, bcVec);
                DirectX::XMVECTOR dotVec2 = DirectX::XMVector3Dot(crossVec2, normalVec);
                float dot2;
                DirectX::XMStoreFloat(&dot2, dotVec2);
                if (dot2 < 0.0f)
                {
                    continue;
                }

                DirectX::XMVECTOR pcVec = DirectX::XMVectorSubtract(cVec, hitPosVec);
                DirectX::XMVECTOR crossVec3 = DirectX::XMVector3Cross(pcVec, caVec);
                DirectX::XMVECTOR dotVec3 = DirectX::XMVector3Dot(crossVec3, normalVec);
                float dot3;
                DirectX::XMStoreFloat(&dot3, dotVec3);
                if (dot3 < 0.0f)
                {
                    continue;
                }

                // 最近距離を更新
                localRayLength = length;

                // マテリアル番号を更新
                materialIndex = subset.materialUniqueID;

                // 交点と法線を更新
                hitPosition = hitPosVec;
                hitNormal = normalVec;
            }
        }

        if (materialIndex >= 0)
        {
            // 交点座標をローカル空間からワールド空間へ変換
            DirectX::XMVECTOR worldPositionVec = DirectX::XMVector3TransformCoord(hitPosition, worldTransformMat);
            // ワールド空間上でのレイの始点から交点までのベクトル
            DirectX::XMVECTOR worldVec = DirectX::XMVectorSubtract(worldPositionVec, worldRayStartVec);
            // ワールド空間上でのレイの視点から交点までの長さ
            DirectX::XMVECTOR worldLengthVec = DirectX::XMVector3Length(worldVec);
            float distance;
            DirectX::XMStoreFloat(&distance, worldLengthVec);

            // ヒット結果情報保存
            if (result.distance > distance)
            {
                // ヒット時の面の法線をローカル空間からワールド空間へ変換
                DirectX::XMVECTOR worldNormal = DirectX::XMVector3TransformNormal(hitNormal, worldTransformMat);

                result.distance = distance;
                result.materialIndex = materialIndex;
                DirectX::XMStoreFloat3(&result.position, worldPositionVec);
                DirectX::XMStoreFloat3(&result.normal, DirectX::XMVector3Normalize(worldNormal));
                hit = true;
            }
        }
    }

    return hit;
}
