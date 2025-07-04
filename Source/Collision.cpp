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

inline DirectX::XMFLOAT3 GetTriangleNormVector(DirectX::XMFLOAT3 p0, DirectX::XMFLOAT3 p1, DirectX::XMFLOAT3 p2)
{
    DirectX::XMFLOAT3 n = {};

    DirectX::XMVECTOR a = DirectX::XMVectorSubtract(XMLoadFloat3(&p1), XMLoadFloat3(&p0));
    DirectX::XMVECTOR b = DirectX::XMVectorSubtract(XMLoadFloat3(&p2), XMLoadFloat3(&p0));
    DirectX::XMStoreFloat3(&n, DirectX::XMVector3Normalize(DirectX::XMVector3Cross(a, b)));

    return n;
}

bool RayVsTriangle(DirectX::XMFLOAT3* p, const Ray& r, const Triangle& t, float& neart)
{
   
    using namespace DirectX;

    constexpr float EPSILON = 1e-6f;

    XMVECTOR orig = XMLoadFloat3(&r.p);
    XMVECTOR dir = XMLoadFloat3(&r.d);
    XMVECTOR v0 = XMLoadFloat3(&t.p0);
    XMVECTOR v1 = XMLoadFloat3(&t.p1);
    XMVECTOR v2 = XMLoadFloat3(&t.p2);

    // 辺ベクトル
    XMVECTOR edge1 = XMVectorSubtract(v1, v0);
    XMVECTOR edge2 = XMVectorSubtract(v2, v0);

    // 計算用ベクトル
    XMVECTOR h = XMVector3Cross(dir, edge2);
    float a = XMVectorGetX(XMVector3Dot(edge1, h));

    // レイが三角形と平行な場合
    if (fabs(a) < EPSILON)
        return false;

    float f = 1.0f / a;
    XMVECTOR s = XMVectorSubtract(orig, v0);
    float u = f * XMVectorGetX(XMVector3Dot(s, h));

    if (u < 0.0f || u > 1.0f)
        return false;

    XMVECTOR q = XMVector3Cross(s, edge1);
    float v = f * XMVectorGetX(XMVector3Dot(dir, q));

    if (v < 0.0f || u + v > 1.0f)
        return false;

    float tempT = f * XMVectorGetX(XMVector3Dot(edge2, q));

    if (tempT > EPSILON && tempT < neart)
    {
        // 交点座標を計算
        XMVECTOR hitPoint = XMVectorAdd(orig, XMVectorScale(dir, tempT));
        XMStoreFloat3(p, hitPoint);
        neart = tempT;
        return true;
    }

    return false;
}

bool SphereVsTriangle(DirectX::XMFLOAT3* p, const Sphere& s, const Triangle& t, float& neart)
{
    using namespace DirectX;

    constexpr float EPSILON = 1e-6f;

    XMVECTOR orig = XMLoadFloat3(&s.p);
    XMVECTOR dir = XMLoadFloat3(&s.d);
    float radius = s.r;

    XMVECTOR v0 = XMLoadFloat3(&t.p0);
    XMVECTOR v1 = XMLoadFloat3(&t.p1);
    XMVECTOR v2 = XMLoadFloat3(&t.p2);

    // 三角形の辺
    XMVECTOR edge1 = XMVectorSubtract(v1, v0);
    XMVECTOR edge2 = XMVectorSubtract(v2, v0);
    XMVECTOR normal = XMVector3Normalize(XMVector3Cross(edge1, edge2));

    // 球心から三角形面までの距離
    float distToPlane = XMVectorGetX(XMVector3Dot(v0 - orig, normal));
    float dot = XMVectorGetX(XMVector3Dot(dir, normal));

    if (fabs(dot) < EPSILON)
        return false; // 平行なので衝突しない

    // 球の表面が三角形面に接触する時の中心点までの距離
    float tHit = (distToPlane - radius) / dot;

    if (tHit < 0.0f || tHit >= neart)
        return false;

    // 球の中心位置（移動後）
    XMVECTOR hitCenter = orig + dir * tHit;

    // 接触点は法線方向に半径戻す
    XMVECTOR contactPoint = hitCenter - normal * radius;

    // 三角形内にあるか判定（バリツール）
    XMVECTOR c0 = XMVector3Cross(v1 - v0, contactPoint - v0);
    XMVECTOR c1 = XMVector3Cross(v2 - v1, contactPoint - v1);
    XMVECTOR c2 = XMVector3Cross(v0 - v2, contactPoint - v2);

    if (XMVectorGetX(XMVector3Dot(c0, normal)) >= 0 &&
        XMVectorGetX(XMVector3Dot(c1, normal)) >= 0 &&
        XMVectorGetX(XMVector3Dot(c2, normal)) >= 0)
    {
        XMStoreFloat3(p, contactPoint);
        neart = tHit;
        return true;
    }

    return false;
}

// レイとモデルの交差判定
bool Collision::IntersectRayVsModel(
    const DirectX::XMFLOAT3& start,
    const DirectX::XMFLOAT3& end,
    const Model* model,
    DirectX::XMFLOAT4X4 modelTransform,
    HitResult& result)
{
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
    for (const Model::Mesh& mesh : model->meshes)
    {

        // AABBの最小・最大点を取得
        DirectX::XMFLOAT3 meshMin = mesh.boundingBox[0];
        DirectX::XMFLOAT3 meshMax = mesh.boundingBox[1];

        // AABBの交差判定
        float tMin = 0.0f, tMax = 1.0f;
        AABB meshAABB = { meshMin, meshMax };
        if (!meshAABB.IntersectsRay(start, end, tMin, tMax))
        {
            continue;  // AABBとレイが交差しない場合、このメッシュをスキップ
        }

        // メッシュ単体の transform
        DirectX::XMMATRIX worldTransformMat = DirectX::XMMatrixMultiply(DirectX::XMLoadFloat4x4(&mesh.defaultGlobalTransform), DirectX::XMLoadFloat4x4(&modelTransform));
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
        const std::vector<Model::vertex>& vertices = mesh.vertices;
        const std::vector<UINT>& indices = mesh.indices;

        // 候補となる情報
        int materialIndex = -1;
        DirectX::XMVECTOR hitPosition;
        DirectX::XMVECTOR hitNormal;

        for (const Model::Mesh::Subset& subset : mesh.subsets)
        {
            for (UINT i = 0; i < subset.indexCount; i += 3)
            {
                UINT index = subset.startIndexLocation + i;

                // 三角形の頂点の抽出
                const Model::vertex& a = vertices.at(indices.at(index));
                const Model::vertex& b = vertices.at(indices.at(index + 1));
                const Model::vertex& c = vertices.at(indices.at(index + 2));

                // レイと三角形の交点の座標（出力用）
                DirectX::XMFLOAT3 p;

                Ray r;
                DirectX::XMStoreFloat3(&r.p, localRayStartVec);
                DirectX::XMStoreFloat3(&r.d, localRayDirectVec);
                DirectX::XMStoreFloat(&r.l, localRayLengthVec);

                Triangle t;
                t.p0 = a.position;
                t.p1 = b.position;
                t.p2 = c.position;

                if (RayVsTriangle(&p, r, t, localRayLength))
                {

                    // マテリアル番号を更新
                    materialIndex = subset.materialUniqueID;

                    // 交点と法線を更新
                    hitPosition = DirectX::XMLoadFloat3(&p);
                    DirectX::XMFLOAT3 n = GetTriangleNormVector(t.p0, t.p1, t.p2);
                    hitNormal = DirectX::XMLoadFloat3(&n);
                }
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

bool Collision::IntersectSphereVsModel(
    const DirectX::XMFLOAT3& start,
    const DirectX::XMFLOAT3& end,
    const float radius,
    const Model* model,
    DirectX::XMFLOAT4X4 modelTransform,
    HitResult& result)
{
    using namespace DirectX;

    XMVECTOR worldStart = XMLoadFloat3(&start);
    XMVECTOR worldEnd = XMLoadFloat3(&end);
    XMVECTOR worldDir = XMVectorSubtract(worldEnd, worldStart);
    float worldLength = XMVectorGetX(XMVector3Length(worldDir));
    if (worldLength <= 0.0f) return false;
    XMVECTOR worldDirN = XMVector3Normalize(worldDir);

    bool hit = false;
    result.distance = FLT_MAX;

    for (const Model::Mesh& mesh : model->meshes)
    {
        // モデルのワールド行列と逆行列
        XMMATRIX worldMat = XMMatrixMultiply(XMLoadFloat4x4(&mesh.defaultGlobalTransform), XMLoadFloat4x4(&modelTransform));
        XMMATRIX invWorldMat = XMMatrixInverse(nullptr, worldMat);

        // レイをローカル空間に変換
        XMVECTOR localStart = XMVector3TransformCoord(worldStart, invWorldMat);
        XMVECTOR localEnd = XMVector3TransformCoord(worldEnd, invWorldMat);
        XMVECTOR localDir = XMVectorSubtract(localEnd, localStart);
        float localLength = XMVectorGetX(XMVector3Length(localDir));
        if (localLength <= 0.0f) continue;
        XMVECTOR localDirN = XMVector3Normalize(localDir);

        // 検査に使うデータ
        const auto& vertices = mesh.vertices;
        const auto& indices = mesh.indices;

        DirectX::XMFLOAT3 hitPos{};
        DirectX::XMFLOAT3 hitNorm{};
        int hitMatIndex = -1;
        float closest = localLength;

        for (const auto& subset : mesh.subsets)
        {
            for (UINT i = 0; i < subset.indexCount; i += 3)
            {
                UINT index = subset.startIndexLocation + i;
                const auto& a = vertices.at(indices.at(index));
                const auto& b = vertices.at(indices.at(index + 1));
                const auto& c = vertices.at(indices.at(index + 2));

                Triangle tri = { a.position, b.position, c.position };
                Sphere movingSphere;
                XMStoreFloat3(&movingSphere.p, localStart);
                XMStoreFloat3(&movingSphere.d, localDirN);
                movingSphere.r = radius;

                float t = closest;
                XMFLOAT3 localHit{};
                if (SphereVsTriangle(&localHit, movingSphere, tri, t))
                {
                    if (t < closest)
                    {
                        closest = t;
                        hitPos = localHit;
                        hitNorm = GetTriangleNormVector(tri.p0, tri.p1, tri.p2);
                        hitMatIndex = subset.materialUniqueID;
                    }
                }
            }
        }

        // ローカル空間でヒットしていたらワールドに変換
        if (hitMatIndex >= 0)
        {
            XMVECTOR hitPosVec = XMLoadFloat3(&hitPos);
            XMVECTOR hitNormVec = XMLoadFloat3(&hitNorm);
            XMVECTOR worldHitPos = XMVector3TransformCoord(hitPosVec, worldMat);
            XMVECTOR worldHitNorm = XMVector3TransformNormal(hitNormVec, worldMat);

            XMVECTOR vecFromStart = XMVectorSubtract(worldHitPos, worldStart);
            float dist = XMVectorGetX(XMVector3Length(vecFromStart));

            if (dist < result.distance)
            {
                result.position = {};
                result.normal = {};
                result.materialIndex = hitMatIndex;
                result.distance = dist;
                XMStoreFloat3(&result.position, worldHitPos);
                XMStoreFloat3(&result.normal, XMVector3Normalize(worldHitNorm));
                hit = true;
            }
        }
    }

    return hit;
}
