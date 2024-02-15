import { lusitana } from '@/app/ui/fonts';

export default function UdawaLogo() {
  return (
    <div className={`${lusitana.className} flex flex-row items-center leading-none text-white`}>
      <img src="/logo.svg" />
      <p className="text-[44px]">UDAWA</p>
    </div>
  );
}